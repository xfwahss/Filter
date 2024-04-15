#ifndef NITRIFICATION_H
#define NITRIFICATION_H
#include <cmath>

// ra  = ra0 + fox * k1a * Cam
// **表明需要随时间步更新，属于隐变量
struct nitrify_status {
    double ra0;
    double kab1;
    double foxmin;
    double c_oxc;
    double c_oxo;
    double theta_a;
    double T_c;
    // 定义构造函数方便初始化
    nitrify_status() = default;
    nitrify_status(const double &ra0, const double &kab1, const double &foxmin,
                   const double &c_oxc, const double &c_oxo,
                   const double &theta_a, const double &T_c)
        : ra0(ra0), kab1(kab1), foxmin(foxmin), c_oxc(c_oxc), c_oxo(c_oxo),
          theta_a(theta_a), T_c(T_c){};
};

class Nitrification {
  private:
    // double ra;      // 硝化过程反应速率
    double ra0;     // 零阶反应速率 **
    double kab1;    // 一阶反应速率常数，20度 **
    double foxmin;  // 最小氧限制系数 **
    double c_oxc;   // 临界溶解氧浓度 **
    double c_oxo;   // 最佳溶解氧浓度 **
    double theta_a; // 温度系数 **
    double T_c;     // 临界温度 **
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
    nitrify_status current_status();
    // 卡尔曼预测步给定新的状态
    void update_status(const nitrify_status &updated_status);
};

#endif