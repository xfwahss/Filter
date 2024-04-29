#ifndef NITRIFICATION_H
#define NITRIFICATION_H
#include "DataStructures.h"
#include <cmath>

// ra  = ra0 + fox * k1a * Cam
class Nitrification {
  private:
    nitri_status status;
    double alpha;   // 曲率系数

    /***************************************************************
     *  @brief    计算溶解氧限制系数
     *  @param     c_ox, 溶解氧浓度
     *  @note      无
     *  @Sample usage:     函数的使用方法
     **************************************************************/
    double fox(const double &c_ox);
    double ka1(const double &T);

  public:
    Nitrification();
    ~Nitrification();
    void init(const double &ra0, const double &kab1, const double &foxmin,
              const double &c_oxc, const double &c_oxo, const double &theta_a,
              const double &T_c, const double &alpha = 0);
    double rate(const double &c_ox, const double &T, const double &c_am);
    // 获取当前的状态给卡尔曼滤波
    nitri_status get_status();
    // 卡尔曼预测步给定新的状态
    void update(const nitri_status &updated_status);
};

#endif