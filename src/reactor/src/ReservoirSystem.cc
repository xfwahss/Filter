#include <ReservoirSystem.h>
#include <logger.h>

RiverGroup *ReservoirSystem::in_ptr        = nullptr;
RiverGroup *ReservoirSystem::out_ptr       = nullptr;
Reservoir *ReservoirSystem::res_ptr        = nullptr;
Ammonification *ReservoirSystem::ammon_ptr = nullptr;
Nitrification *ReservoirSystem::nitri_ptr  = nullptr;
Denitrification *ReservoirSystem::deni_ptr = nullptr;
bool ReservoirSystem::system_inited        = false;
int ReservoirSystem::status_len            = 0;
int ReservoirSystem::bnd_force_len         = 0;
int ReservoirSystem::in_status_len         = 0;
int ReservoirSystem::out_status_len        = 0;
int ReservoirSystem::ammonification_len    = 0;
int ReservoirSystem::nitrification_len     = 0;
int ReservoirSystem::denitrification_len   = 0;

void ReservoirSystem::update_bnd(const Eigen::VectorXd &bnd_force, const int &in_nums, const int &out_nums) {
    if (bnd_force.size() != bnd_force_len) {
        logger::get("reservoir_system")->error("The size of bnd_force_len is not equal to {}", bnd_force_len);
        throw std::runtime_error("wrong size of vectorxd");
    }
    Eigen::VectorXd in_stat    = bnd_force.segment(0, in_status_len);
    Eigen::VectorXd out_stat   = bnd_force.segment(in_status_len, out_status_len);
    Eigen::VectorXd ammon_stat = bnd_force.segment(in_status_len + out_status_len, ammonification_len);
    Eigen::VectorXd nitri_stat =
        bnd_force.segment(in_status_len + out_status_len + ammonification_len, nitrification_len);
    Eigen::VectorXd denitri_stat =
        bnd_force.segment(in_status_len + out_status_len + ammonification_len + nitrification_len, denitrification_len);
    in_ptr->update(in_stat);
    out_ptr->update(out_stat);
    ammon_ptr->update(ammon_stat);
    nitri_ptr->update(nitri_stat);
    deni_ptr->update(denitri_stat);
}

void ReservoirSystem::update_status(const Eigen::VectorXd& status){
    res_ptr->update(status);
}

void ReservoirSystem::init_system(const Eigen::VectorXd &bnd_force, const int &in_nums, const int &out_nums) {
    in_ptr    = new RiverGroup();
    out_ptr   = new RiverGroup();
    res_ptr   = new Reservoir();
    ammon_ptr = new Ammonification();
    nitri_ptr = new Nitrification();
    deni_ptr  = new Denitrification();

    // 各子系统初始化
    for (int i = 0; i < in_nums; ++i) {
        in_ptr->add_river(i + 1);
    }
    for (int i = 0; i < out_nums; ++i) {
        out_ptr->add_river(i + 1);
    }
    res_ptr->init(0, 0, 0, 0, 0, 0);
    ammon_ptr->init(0, 0);
    nitri_ptr->init(0, 0, 0, 0, 0, 0, 0);
    deni_ptr->init(0, 0, 0, 0, 0, 0);

    // 相关状态更新
    status_len          = res_status::size;
    in_status_len       = in_nums * river_status::size;
    out_status_len      = out_nums * river_status::size;
    ammonification_len  = ammon_status::size;
    nitrification_len   = nitri_status::size;
    denitrification_len = deni_status::size;
    bnd_force_len       = in_status_len + out_status_len + ammonification_len + nitrification_len + denitrification_len;
    system_inited       = true;
}

Eigen::VectorXd &ReservoirSystem::predict(Eigen::VectorXd &status, const Eigen::VectorXd &bnd_force, const int &in_nums,
                                          const int &out_nums, const double &dt) {
    if (!system_inited) {
        init_system(bnd_force, in_nums, out_nums);
    }
    update_bnd(bnd_force, in_nums, out_nums);
    update_status(status);

    res_status res_s = status;
    Eigen::VectorXd in_flows = in_ptr->get_status();
    Eigen::VectorXd out_flows = out_ptr->get_status();
    double ro = ammon_ptr->rate(res_s.c_no);
    double ra = nitri_ptr->rate(res_s.c_do, res_s.T, res_s.c_na);
    double rn = deni_ptr->rate(res_s.c_do, res_s.T, res_s.c_nn);
    status = res_ptr->predict(in_flows, out_flows, ro, ra, rn, dt);
    return status;
}