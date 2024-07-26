#include <ReservoirSystem.h>
#include <logger.h>

RiverGroup ReservoirSystem::in;
RiverGroup ReservoirSystem::out;
WaterColumn ReservoirSystem::res;
Ammonification ReservoirSystem::ammon;
Nitrification ReservoirSystem::nitri;
Denitrification ReservoirSystem::deni;
Sediment ReservoirSystem::sedi;
ParticleSettle ReservoirSystem::particles;
bool ReservoirSystem::system_inited      = false;
int ReservoirSystem::status_len          = 0;
int ReservoirSystem::bnd_force_len       = 0;
int ReservoirSystem::in_status_len       = 0;
int ReservoirSystem::out_status_len      = 0;
int ReservoirSystem::ammonification_len  = 0;
int ReservoirSystem::nitrification_len   = 0;
int ReservoirSystem::denitrification_len = 0;
int ReservoirSystem::sediment_len        = 0;
int ReservoirSystem::particlesettle_len  = 0;
int ReservoirSystem::parameters_len      = 0;

void ReservoirSystem::init_system(const int &in_nums, const int &out_nums) {
    // 各子系统初始化
    for (int i = 0; i < in_nums; ++i) {
        in.add_river();
    }
    for (int i = 0; i < out_nums; ++i) {
        out.add_river();
    }
    // 所有系统参数置0
    Eigen::VectorXd in_0 =
        Eigen::VectorXd::Zero(River::status_nums * in.get_river_nums());
    Eigen::VectorXd out_0 =
        Eigen::VectorXd::Zero(River::status_nums * out.get_river_nums());
    Eigen::VectorXd res_0   = Eigen::VectorXd::Zero(WaterColumn::status_nums);
    Eigen::VectorXd ammon_0 = Eigen::VectorXd::Zero(Ammonification::param_nums);
    Eigen::VectorXd nitri_0 = Eigen::VectorXd::Zero(Nitrification::param_nums);
    Eigen::VectorXd deni_0 = Eigen::VectorXd::Zero(Denitrification::param_nums);
    Eigen::VectorXd sedi_0 = Eigen::VectorXd::Zero(Sediment::param_nums);
    Eigen::VectorXd particles_0 =
        Eigen::VectorXd::Zero(ParticleSettle::param_nums);
    in.update(in_0);
    out.update(out_0);
    res.update(res_0);
    ammon.update(ammon_0);
    nitri.update(nitri_0);
    deni.update(deni_0);
    sedi.update(sedi_0);
    particles.update(particles_0);

    // 相关状态更新
    status_len          = WaterColumn::status_nums;
    in_status_len       = in_nums * River::status_nums;
    out_status_len      = out_nums * River::status_nums;
    ammonification_len  = Ammonification::param_nums;
    nitrification_len   = Nitrification::param_nums;
    denitrification_len = Denitrification::param_nums;
    sediment_len        = Sediment::param_nums;
    particlesettle_len  = ParticleSettle::param_nums;
    bnd_force_len       = in_status_len + out_status_len;
    parameters_len      = ammonification_len + nitrification_len +
                     denitrification_len + sediment_len + particlesettle_len;
    system_inited = true;
}

void ReservoirSystem::set_wl_volumn_function(
    std::function<double(const double &wl)> wl_to_volumn,
    std::function<double(const double &wl)> wl_to_depth,
    std::function<double(const double &volumn)> volumn_to_wl) {
    res.update_wl_volumn_function(wl_to_volumn, wl_to_depth, volumn_to_wl);
}

void ReservoirSystem::update_status(const Eigen::VectorXd &s) { res.update(s); }
void ReservoirSystem::update_bnd(const Eigen::VectorXd &bnd_force) {
    if (bnd_force.size() != bnd_force_len) {
        Logger::get("Reservoir_system")
            ->error("The size of bnd_force_len is not equal to {}",
                    bnd_force_len);
        throw std::runtime_error("Wrong size of vectorxd");
    }
    Eigen::VectorXd in_stat  = bnd_force.segment(0, in_status_len);
    Eigen::VectorXd out_stat = bnd_force.segment(in_status_len, out_status_len);
    in.update(in_stat);
    out.update(out_stat);
}

void ReservoirSystem::update_params(const Eigen::VectorXd &params) {
    if (params.size() != parameters_len) {
        Logger::get("Reservoir_system")
            ->error("The size of params is not equal to {}", parameters_len);
        throw std::runtime_error("Wrong size of vectorxd");
    } else {
        Eigen::VectorXd ammon_stat = params.segment(0, ammonification_len);
        Eigen::VectorXd nitri_stat =
            params.segment(ammonification_len, nitrification_len);
        Eigen::VectorXd deni_stat = params.segment(
            ammonification_len + nitrification_len, denitrification_len);
        Eigen::VectorXd sediment_stat = params.segment(
            ammonification_len + nitrification_len + denitrification_len,
            sediment_len);
        Eigen::VectorXd particles_stat =
            params.segment(ammonification_len + nitrification_len +
                               denitrification_len + sediment_len,
                           particlesettle_len);
        ammon.update(ammon_stat);
        nitri.update(nitri_stat);
        deni.update(deni_stat);
        sedi.update(sediment_stat);
        particles.update(particles_stat);
    }
}

Eigen::VectorXd ReservoirSystem::predict(const Eigen::VectorXd &status,
                                         const Eigen::VectorXd &bnd_force,
                                         const Eigen::VectorXd &params,
                                         const double &dt) {
    update_status(status);
    update_bnd(bnd_force);
    update_params(params);

    Eigen::VectorXd in_flows  = in.flow_status();
    Eigen::VectorXd out_flows = out.flow_status();
    Eigen::VectorXd flow_bnd(in_flows.size() + out_flows.size());
    flow_bnd << in_flows, out_flows;

    Eigen::VectorXd sediment_bnd  = sedi.sediment_flux();
    Eigen::VectorXd settle_params = particles.get_settle_params();

    Eigen::VectorXd next_status   = res.predict(
        status, dt, flow_bnd, sediment_bnd, settle_params, ammon, nitri, deni);
    return next_status;
}