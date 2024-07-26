#ifndef DENITRIFICATION_H
#define DENITRIFICATION_H
#include <Eigen/Dense>

class Denitrification {
  private:
    double r_deni0;
    double k_deni_20;
    double Tc;
    double theta;
    double c_oxc;
    double c_oxo;
    double beta;
    double fox(const double &c_ox);
    double ft(const double &T);

  public:
    static const int param_nums;
    Denitrification();
    ~Denitrification();

    void init(const double &r_deni0, const double &k_deni_20, const double &Tc,
              const double &theta, const double &c_oxc, const double &c_oxo,
              const double &beta = 1.0);
    double rate0();
    double k_deni(const double &c_ox, const double &T);
    /* @brief VectorXd 用于更新反硝化系统的状态，s的数据格式为
     * [r_deni0, k_deni_20, Tc, theta, c_oxc, c_oxo, beta]
     */
    void update(const Eigen::VectorXd &s);
};
#endif