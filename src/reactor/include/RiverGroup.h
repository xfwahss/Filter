#ifndef RIVER_GROUP_H
#define RIVER_GROUP_H
#include <River.h>
#include <map>

class RiverGroup{
  private:
    std::map<int, River *> rivers;
    // 用于统计河流的数目
    int river_nums;

  public:
    RiverGroup();
    ~RiverGroup();
    void add_river();
    // Eigen 给定每一条河流 flow, c_organci, c_ammonia, c_nitrate, 依次
    /* @brief 用于更新所有河流的状态
    * @param VectorXd s, s的格式应该如下:
    * [flow, load_rpon, load_lpon, load_don, load_na, load_na] * n条河流
    */
    void update(const Eigen::VectorXd &s);
    Eigen::VectorXd flow_status();
    double get_river_nums();
};
#endif