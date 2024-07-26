#include <ReservoirSystem.h>
#include <iostream>

double wl_to_volume(const double &wl) { return 10 * wl; }
double wl_to_depth(const double &wl) { return wl; }
double volumn_to_wl(const double &volumn) { return volumn / 10; }
int main() {
    ReservoirSystem::init_system(2, 2);
    ReservoirSystem::set_wl_volumn_function(wl_to_volume, wl_to_depth,
                                            volumn_to_wl);
    Eigen::VectorXd status(8);
    status << 1, 1, 1, 1, 1, 1, 15, 10;
    Eigen::VectorXd bnd_force(4 * 6);
    bnd_force << 1, 1, 1, 1, 1, 1, // river_in1
        1, 1, 1, 1, 1, 1,          // river_in2
        1, 1, 1, 1, 1, 1,          // river_out1
        1, 1, 1, 1, 1, 1;          // river_out2
    Eigen::VectorXd params(26);
    params << 0.1, 1, 0.1, 1, 0.1, 1,  // ammonification
        0.1, 0.1, 0.1, 2, 8, 1, 15, 0, // nitrification
        0.1, 0.1, 15, 1, 8, 2, 1,      // denitrification
        0.1, 0.1, 0.1,                 // sediment
        0.1, 0.1;                      // settle
    double dt = 86400;
    Eigen::VectorXd next_s = ReservoirSystem::predict(status, bnd_force, params, dt);
    std::cout << next_s.transpose();
}