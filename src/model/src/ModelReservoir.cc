#include "../../reactor/include/Ammonification.h"
#include "../../reactor/include/Denitrification.h"
#include "../../reactor/include/Nitrification.h"
#include "../../reactor/include/Reservoir.h"
#include "../../reactor/include/RiverSystem.h"

class ReservoirModel {
  private:
    RiverSystem rivers_in;
    RiverSystem rivers_out;
    Reservoir res;
    Ammonification ammon;
    Nitrification nitri;
    Denitrification deni;

  public:
    ReservoirModel()  = default;
    ~ReservoirModel() = default;
    ReservoirModel(RiverSystem &rivers_in, RiverSystem &rivers_out,
                   Reservoir &res, Ammonification &ammon, Nitrification &nitri,
                   Denitrification &deni);
    void init();
    void predict(const double &dt, const Eigen::VectorXd &river_in_stat,
                 const Eigen::VectorXd &river_out_stat, ammon_status ammon_stat,
                 nitri_status ni_stat, deni_status deni_stat,
                 const double &res_T, const double &res_do);
    void simulate(const double &dt, const int &steps);
};

ReservoirModel::ReservoirModel(RiverSystem &rivers_in, RiverSystem &rivers_out,
                               Reservoir &res, Ammonification &ammon,
                               Nitrification &nitri, Denitrification &deni)
    : rivers_in(rivers_in), rivers_out(rivers_out), res(res), ammon(ammon),
      nitri(nitri), deni(deni) {}

void ReservoirModel::init() {}

void ReservoirModel::predict(const double &dt,
                             const Eigen::VectorXd &river_in_stat,
                             const Eigen::VectorXd &river_out_stat,
                             ammon_status ammon_stat, nitri_status ni_stat,
                             deni_status deni_stat, const double &res_T,
                             const double &res_do) {

    // 入库河流状态更新
    rivers_in.update(river_in_stat);
    double flow_in, load_org_in, load_amm_in, load_nit_in;
    flow_in     = rivers_in.get_status()(0);
    load_org_in = rivers_in.get_status()(1);
    load_amm_in = rivers_in.get_status()(2);
    load_nit_in = rivers_in.get_status()(3);

    // 出库河流状态更新
    rivers_out.update(river_out_stat);
    double flow_out, load_org_out, load_amm_out, load_nit_out;
    flow_out     = rivers_out.get_status()(0);
    load_org_out = rivers_out.get_status()(1);
    load_amm_out = rivers_out.get_status()(2);
    load_nit_out = rivers_out.get_status()(3);

    // 水库状态
    res_status res_cur = res.get_status();

    // 反应系统更新
    ammon.update(ammon_stat);
    nitri.update(ni_stat);
    deni.update(deni_stat);
    double ro = ammon.rate(res_cur.c_no);
    double rn = deni.rate(res_cur.c_do, res_cur.T, res_cur.c_nn);
    double ra = nitri.rate(res_cur.c_do, res_cur.T, res_cur.c_na);

    res.predict(dt, flow_in, flow_out, load_org_in, load_org_out, load_amm_in,
                load_amm_out, load_nit_in, load_nit_out, ro, ra, rn);
    // 水库T和DO更新
    res_status update_res(res.get_status().wl, res.get_status().c_no,
                          res.get_status().c_na, res.get_status().c_nn, res_T,
                          res_do);
    res.update(update_res);
}

int main() {
    River baihe, chaohe, baihe_out, chaohe_out;
    RiverSystem rivers_in, rivers_out;
    rivers_in.add_river("Baihe", baihe);
    rivers_in.add_river("Chaohe", chaohe);
    rivers_out.add_river("Baihe_Dam", baihe_out);
    rivers_out.add_river("Chaohe_Dam", chaohe_out);
    Reservoir res;
    Ammonification ammon;
    Nitrification nitri;
    Denitrification deni;
    ReservoirModel model(rivers_in, rivers_out, res, ammon, nitri, deni);
    return 0;
}