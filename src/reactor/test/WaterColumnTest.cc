#include <Ammonification.h>
#include <Denitrification.h>
#include <Nitrification.h>
#include <WaterColumn.h>
#include <iostream>

double wl_to_volume(const double &wl) { return 10 * wl; }
double wl_to_depth(const double &wl) { return wl; }
double volume_to_wl(const double &volumn) { return volumn / 10; }
int main() {
    WaterColumn wc(wl_to_volume, wl_to_depth, volume_to_wl);
    Eigen::VectorXd init_s(WaterColumn::status_nums);
    init_s << 2, 1, 1, 1, 1, 1, 15, 10;
    Eigen::VectorXd flow_bnd(WaterColumn::flow_bnd_nums);
    flow_bnd << 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1;
    Eigen::VectorXd sediment_bnd(WaterColumn::sediment_bnd_nums);
    sediment_bnd << 1, 1, 1;
    Eigen::VectorXd settle_params(WaterColumn::settle_param_nums);
    settle_params << 1, 1;
    Ammonification ammon;
    Nitrification nitri;
    Denitrification deni;
    ammon.init(0.1, 1, 0.1, 1, 0.1, 1);
    nitri.init(0.1, 0.1, 0.5, 2, 10, 1, 10, 0);
    deni.init(0.1, 0.1, 10, 1, 8, 2, 1);
    Eigen::VectorXd next_s = wc.predict(init_s, 86400, flow_bnd, sediment_bnd,
                                        settle_params, ammon, nitri, deni);
    std::cout << next_s.transpose();
}