#ifndef DENITRIFICATION_H
#define DENITRIFICATION_H
#define _USE_MATH_DEFINES
#include "DataStructures.h"
#include <cmath>

class Denitrification {
  private:
    deni_status coeffecients;
    double beta; // 曲率系数，溶解氧限制函数曲率
    double fnox(const double &c_ox);
    double kn1(const double &T);

  public:
    Denitrification();
    ~Denitrification();

    /***************************************************************
     *  @brief    初始化反硝化过程状态
     *  @param    rn0, 0阶反应速率
     *  @param    knb1, 20度时的反硝化速率常数
     *  @param    Tnc, 临界温度
     *  @param    theta_n, 温度系数
     *  @param    c_noxc, 临界溶解氧浓度
     *  @param    c_noxo, 最佳溶解氧浓度
     *  @param    beta, 曲率系数，溶解氧限制曲线 beta=1时为直线
     *  @Sample usage:     函数的使用方法
     **************************************************************/
    void init(const double &rn0, const double &knb1, const double &Tnc,
              const double &theta_n, const double &c_noxc, const double &c_noxo,
              const double &beta = 1.0);
    /***************************************************************
     *  @brief    依据溶解氧浓度，硝态氮浓度，温度计算反硝化速率
     *  @param    c_ox, 溶解氧浓度
     *  @param    T, 温度
     *  @param    c_ni, 硝态氮浓度
     *  @Sample usage:     函数的使用方法
     **************************************************************/
    double rate(const double &c_ox, const double &T, const double &c_ni);
    // 获取当前系统中的反硝化状态
    deni_status get_status();
    // 更新当前系统中的反硝化状态
    void update(const deni_status &updated_status);
    void update(const Eigen::VectorXd &status);
};
#endif