#ifndef U_MATH_H
#define U_MATH_H
#include <Eigen/Dense>

// user_math
namespace umath {
bool is_positive(const Eigen::VectorXd &value);
double randomd(const double &mean, const double &variance);
double fill_missed_value(const double &value,
                         const double &missed_value = -999.0,
                         const double &filled_value = 0);

// 多元高斯分布随机向量
Eigen::VectorXd multivariate_gaussian_random(Eigen::VectorXd &mean,
                                             Eigen::MatrixXd &covariance);

// 向量全部为正值的多元高斯分布
Eigen::VectorXd pos_multi_gauss_random(Eigen::VectorXd &mean,
                                             Eigen::MatrixXd &covariance);
}; // namespace umath

#endif