#ifndef RIVER_GROUP_H
#define RIVER_GROUP_H
#include <River.h>
#include <map>

class RiverGroup{
  private:
    std::map<int, River *> rivers;
    // 水系状态仅存储 [总流量,有机氮,氨氮负荷,硝氮负荷]
    Eigen::VectorXd status;
    Eigen::VectorXd sum();

  public:
    RiverGroup();
    ~RiverGroup();
    void add_river(const int& id);
    Eigen::VectorXd get_status();
    // Eigen 给定每一条河流 flow, c_organci, c_ammonia, c_nitrate, 依次
    void update(Eigen::VectorXd rivers_status);
};
#endif