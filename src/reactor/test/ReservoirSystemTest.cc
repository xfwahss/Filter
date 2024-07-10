#include <ReservoirSystem.h>

int main(){
    Eigen::VectorXd status(6);
    status << 136.33, 1, 1, 1, 30, 9;
    Eigen::VectorXd bnd_force(31);
    bnd_force << 1.20, 1, 1, 1,     1.68, 1, 1, 1,
                 0.00, 1, 1, 1,     8.75, 1, 1.2, 1,
                 0, 0,
                 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0;
    Eigen::VectorXd next_status = ReservoirSystem::predict(status, bnd_force, 2, 2, 1);
    std::cout << next_status.transpose() << std::endl;

    Eigen::VectorXd bnd_force2(31);
    bnd_force2 << 1.40, 1, 1, 1,     1.81, 1, 1, 1,
                 0.00, 1, 1, 1,     8.75, 1, 1.2, 1,
                 0, 0,
                 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0;
    Eigen::VectorXd next_status2 = ReservoirSystem::predict(next_status, bnd_force2, 2, 2, 1);
    std::cout << next_status2.transpose() << std::endl;
}