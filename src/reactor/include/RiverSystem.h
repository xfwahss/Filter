#ifndef RIVER_SYSTEM_H
#define RIVER_SYSTEM_H
#include "River.h"
#include <iostream>
#include <map>

class RiverSystem {
  private:
    std::map<std::string, River *> rivers;
    // 水系状态仅存储 [总流量,有机氮,氨氮负荷,硝氮负荷]
    Eigen::VectorXd status;
    Eigen::VectorXd sum();

  public:
    RiverSystem();
    ~RiverSystem();
    void add_river(std::string river_name, River &river);
    Eigen::VectorXd get_status();
    // Eigen 给定每一条河流 flow, c_organci, c_ammonia, c_nitrate, 依次
    void update(Eigen::VectorXd rivers_status);
    friend std::ostream &operator<<(std::ostream &os,
                                    const RiverSystem &rivers);
};
#endif