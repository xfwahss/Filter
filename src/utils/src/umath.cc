#include "../include/umath.h"
#include <random>
#include <iostream>

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

double umath::fill_missed_value(const double &value, const double &missed_value,
                                const double &fill_value) {
    if (value == missed_value) {
        return fill_value;
    } else {
        return value;
    }
}

Eigen::VectorXd
umath::multivariate_gaussian_random(Eigen::VectorXd &mean,
                                    Eigen::MatrixXd &covariance) {
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

Eigen::VectorXd
umath::pos_multi_gauss_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance){
    int count = 0;
    Eigen::VectorXd random_vector = umath::multivariate_gaussian_random(mean, covariance);
    while(!is_positive(random_vector)){
        random_vector = umath::multivariate_gaussian_random(mean, covariance);
        count++;
        // if(count > 1){
        //     std::cout << "Resample "<< count << " times" << std::endl;
        // }
    }
    return random_vector;
}