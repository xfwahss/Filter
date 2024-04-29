#ifndef RIVER_H
#define RIVER_H
#include "DataStructures.h"
class River {
  private:
    river_status status;

  public:
    River();
    ~River();
    void init(const double &flow, const double &c_no, const double &c_na,
              const double &c_nn);
    river_status get_status();
    void update(river_status &status);
    // 用于河流状态预测, 给定逐日监测数据差量的平均值预测
    void predict(const double &dt, const double &d_flow, 
    const double&d_cno, const double&d_cna, const double&d_cnn);
};
#endif