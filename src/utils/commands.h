#ifndef COMMANDS_H
#define COMMANDS_H
#include <Eigen/Dense>
#include <initializer_list>
#include <random>
#include <string>
#include <unordered_map>

namespace tools {
std::unordered_map<std::string, std::string> get_args(int argc, char *argv[]) {
    std::unordered_map<std::string, std::string> options;
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 < argc) {
            // Check if option flag starts with "-"
            if (argv[i][0] == '-') {
                std::string key   = argv[i];
                std::string value = argv[i + 1];
                options[key]      = value;
            }
        }
    }
    return options;
}

double handle_miss(const double &value) {
    if (value == -999.0) {
        return 0;
    } else {
        return value;
    }
}

Eigen::VectorXd multivariate_gaussian_random(Eigen::VectorXd &status,
                                             Eigen::MatrixXd &covariance) {
    std::random_device seed;
    std::mt19937_64 engine(seed());
    std::normal_distribution<double> dist(0.0, 1.0);

    // 使用Eigen库计算Cholesky分解
    Eigen::LLT<Eigen::MatrixXd> lltOfCov(covariance);
    Eigen::MatrixXd L = lltOfCov.matrixL();

    // 生成随机数
    Eigen::VectorXd z(status.size());
    for (int i = 0; i < status.size(); ++i) {
        z(i) = dist(engine);
    }
    Eigen::VectorXd random_number = status + L * z;
    return random_number;
}

double exclude_mean(std::initializer_list<double> args) {
    double sum   = 0;
    double count = 0;
    for (double arg : args) {
        if (arg != -999) {
            sum += arg;
            ++count;
        };
    }
    if (count == 0) {
        return 0;
    } else {
        return sum / count;
    }
}

double random_double(const double &mean, const double &variance) {
    std::random_device seed;
    std::mt19937_64 engine(seed());
    double stddev = std::sqrt(variance);
    std::normal_distribution<double> dist(mean, stddev);
    double value = dist(engine);
    return value;
}

Eigen::MatrixXd covariance(const Eigen::MatrixXd &mat) {
    /* mat的数据存储格式
     * 矩阵的列数为变量的个数
     *矩阵的行数为每个变量的观测值个数
     */
    Eigen::MatrixXd covariance(mat.cols(), mat.cols());
    Eigen::VectorXd sum = Eigen::VectorXd::Zero(mat.cols());
    for (int i = 0; i < mat.cols(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            sum(i) += mat(j, i);
        }
    }
    Eigen::VectorXd mean = sum / mat.rows();
    for (int i = 0; i < mat.rows(); ++i) {
        covariance += (mat.row(i).transpose() - mean) *
                      (mat.row(i).transpose() - mean).transpose();
    }
    return covariance/(mat.rows() - 1);
}
} // namespace tools
#endif