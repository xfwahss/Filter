#ifndef NITRIFICATION_H
#define NITRIFICATION_H
#include <Eigen/Dense>

// ra  = ra0 + fox * k1a * Cam
class Nitrification {
  private:
    double rnit0;
    double knit_20;
    double foxmin;
    double c_oxc;
    double c_oxo;
    double theta;
    double T_c;
    double alpha; // 曲率系数

    double fox(const double &c_ox);
    double ft(const double &T);

  public:
    static const int param_nums;
    Nitrification();
    ~Nitrification();
    void init(const double &rnit0, const double &knit_20, const double &foxmin,
              const double &c_oxc, const double &c_oxo, const double &theta,
              const double &T_c, const double &alpha = 0);
    double rate_0();
    double knit(const double &c_ox, const double &T);
    // 获取当前的状态给卡尔曼滤波

    /* @brief 更新硝化系统状态
     * @param VectorXd s的存储格式: alpha=0为没有修正
     * [rnit0, knit_20, foxmin, c_oxc, c_oxo, theta, T_c, alpha]
     */
    void update(const Eigen::VectorXd &s);
};

#endif