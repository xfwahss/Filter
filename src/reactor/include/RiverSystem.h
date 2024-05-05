#ifndef RIVER_SYSTEM_H
#define RIVER_SYSTEM_H
#include "River.h"
#include <iostream>
#include <map>

class RiverSystem {
  private:
    std::map<std::string, River *> rivers;
    Eigen::VectorXd status;
    Eigen::VectorXd sum();

  public:
    RiverSystem();
    ~RiverSystem();
    void add_river(std::string river_name, River &river);
    Eigen::VectorXd get_status();
    void update(Eigen::VectorXd status);
    void predict(const double &dt);
    friend std::ostream &operator<<(std::ostream &os,
                                    const RiverSystem &rivers);
};
#endif