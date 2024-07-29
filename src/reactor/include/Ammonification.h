#ifndef AMMONIFICATION_H
#define AMMONIFICATION_H
#include <Eigen/Dense>
class Ammonification {
  private:
    double k_lpon_20;  // rate constant for labile particualte organic nitrogen
    double theta_lpon; // temperature coefficient
    double
        k_rpon_20; // rate constant for refactory particulate organic nitrogen
    double theta_rpon; // temperature coefficient
    double k_don_20;   // rate constant for dissolved organic nitrogen
    double theta_don;  // temperature coefficient

  public:
    static const int param_nums;
    Ammonification()  = default;
    ~Ammonification() = default;
    void init(const double &k_rpon_20, const double &theta_rpon,
              const double &k_lpon_20, const double &theta_lpon,
              const double &k_don_20, const double &theta_don);
    // 更新系统参数
    void update(const Eigen::VectorXd &s);
    double k_lpon_hydrolysis(const double &T);
    double k_rpon_hydrolysis(const double &T);
    double k_don_mineralization(const double &T);
};
#endif