#include <Ammonification.h>
const int Ammonification::param_nums = 6;

void Ammonification::init(const double &k_rpon_20, const double &theta_rpon,
                          const double &k_lpon_20, const double &theta_lpon,
                          const double &k_don_20, const double &theta_don) {
    this->k_lpon_20  = k_lpon_20;
    this->theta_lpon = theta_lpon;
    this->k_rpon_20  = k_rpon_20;
    this->theta_rpon = theta_rpon;
    this->k_don_20   = k_don_20;
    this->theta_don  = theta_don;
}
void Ammonification::update(const Eigen::VectorXd &s) {
    if (s.size() == param_nums) {
        k_rpon_20  = s(0);
        theta_rpon = s(1);
        k_lpon_20  = s(2);
        theta_lpon = s(3);
        k_don_20   = s(4);
        theta_don  = s(5);
    } else {
        throw std::length_error("The length of vector input doesn't match the "
                                "param_nums--<Ammonification>");
    }
}
double Ammonification::k_lpon_hydrolysis(const double &T) {
    return k_lpon_20 * std::pow(theta_lpon, T - 20);
}
double Ammonification::k_rpon_hydrolysis(const double &T) {
    return k_rpon_20 * std::pow(theta_rpon, T - 20);
}
double Ammonification::k_don_mineralization(const double &T) {
    return k_don_20 * std::pow(theta_don, T - 20);
}