#ifndef RIVER_SYSTEM_H
#define RIVER_SYSTEM_H
#include "River.h"
#include <vector>

class RiverSystem {
  private:
    std::vector<River *> rivers;
    Eigen::VectorXd status;

  public:
    RiverSystem();
    ~RiverSystem();
    void add_river(River &river);
    Eigen::VectorXd get_status();
    void update(Eigen::VectorXd status);
    void predict(const double& dt);
};
#endif