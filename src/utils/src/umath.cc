#include "../include/umath.h"
#include <random>

bool umath::is_positive(const Eigen::VectorXd &value) {
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        if (value(i) < 0) {
            return false;
        }
    }
    return true;
}

double umath::randomd(const double &mean, const double &variance) {
    std::random_device seed;
    std::mt19937_64 engine(seed());
    double stddev = std::sqrt(variance);
    std::normal_distribution<double> dist(mean, stddev);
    double value = dist(engine);
    return value;
}

double umath::fill_missed_value(const double &value, const double &missed_value, const double &fill_value) {
    if (value == missed_value) {
        return fill_value;
    } else {
        return value;
    }
}

Eigen::VectorXd umath::multivariate_gaussian_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance) {
    // 使用Eigen库计算Cholesky分解
    Eigen::LLT<Eigen::MatrixXd> lltOfCov(covariance);
    Eigen::MatrixXd L = lltOfCov.matrixL();

    Eigen::VectorXd z(mean.size());
    for (int i = 0; i < mean.size(); ++i) {
        z(i) = umath::randomd(0, 1);
    }
    Eigen::VectorXd random_number = mean + L * z;
    return random_number;
}

Eigen::VectorXd umath::pos_multi_gauss_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance) {
    int count                     = 0;
    Eigen::VectorXd random_vector = umath::multivariate_gaussian_random(mean, covariance);
    while (!is_positive(random_vector)) {
        random_vector = umath::multivariate_gaussian_random(mean, covariance);
        count++;
        // if(count > 1){
        //     std::cout << "Resample "<< count << " times" << std::endl;
        // }
    }
    return random_vector;
}


Eigen::MatrixXd umath::covariance(const Eigen::MatrixXd &mat) {
    /* mat的数据存储格式
     * 矩阵的列数为变量的个数
     *矩阵的行数为每个变量的观测值个数
     */
    Eigen::MatrixXd covariance = Eigen::MatrixXd::Zero(mat.cols(), mat.cols());
    Eigen::VectorXd sum = Eigen::VectorXd::Zero(mat.cols());
    for (int i = 0; i < mat.cols(); ++i) {
        for (int j = 0; j < mat.rows(); ++j) {
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


double umath::avg_exclude_nans(std::initializer_list<double> args) {
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


std::unordered_map<std::string, std::string> umath::get_args(int argc, char *argv[]) {
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