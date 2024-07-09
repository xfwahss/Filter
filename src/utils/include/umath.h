#ifndef U_MATH_H
#define U_MATH_H
#include <Eigen/Dense>

// user_math
namespace umath {
// 判断VectorXd中的元素是不是全部大于或等于0
bool is_positive(const Eigen::VectorXd &value);

// 产生均值为mean，方差为variance且类型double的随机数
double randomd(const double &mean, const double &variance);

// 判断值是不是缺失值(-999.0)，如果是，填充为filled_value
double fill_missed_value(const double &value, const double &missed_value = -999.0, const double &filled_value = 0);

// 多元高斯分布随机向量
Eigen::VectorXd multivariate_gaussian_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance);

// 向量全部为正值的多元高斯分布
Eigen::VectorXd pos_multi_gauss_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance);

/* mat的数据存储格式
 * 矩阵的列数为变量的个数
 *矩阵的行数为每个变量的观测值个数
 */
Eigen::MatrixXd covariance(const Eigen::MatrixXd &mat);

// 剔除空值后求均值
double avg_exclude_nans(std::initializer_list<double> args);

std::unordered_map<std::string, std::string> get_args(int argc, char *argv[]);
}; // namespace umath

#endif