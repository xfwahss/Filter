#include "KalmanFilter.h"
#include <iostream>

int main() {
    Eigen::VectorXd x(1); // 初始状态
    Eigen::VectorXd u(1); // 初始状态
    Eigen::MatrixXd P(1, 1); // 初始状态协方差矩阵
    Eigen::MatrixXd B(1, 1); // 初始状态协方差矩阵
    Eigen::MatrixXd F(1, 1); // 状态转移矩阵
    Eigen::MatrixXd H(1, 1); // 观测矩阵
    Eigen::MatrixXd Q(1, 1); // 过程噪声协方差矩阵
    Eigen::MatrixXd R(1, 1); // 测量噪声协方差矩阵

    // 初始化系统参数
    x << 2; // 初始位置
    P << 10; // 初始状态协方差
    F << 1;  // 状态转移为恒等矩阵
    H << 1;  // 观测矩阵为恒等矩阵
    Q << 0.01; // 过程噪声较小
    R << 1;   // 测量噪声较大
    B << 0;
    u << 0;

    KalmanFilter kf(2, 1);
    kf.init(x, P, F, Q, B, u, H, R);

    double measurements[] = {2.1, 2.0, 2.1, 2.5, 2.2, 2.3, 3.5, 2.7, 2.8, 2.4, 2.3};
    size_t num_measurements = sizeof(measurements) / sizeof(measurements[0]);
    kf.batch_filter(num_measurements, measurements);
    kf.batch_filter(num_measurements, measurements);
    kf.batch_filter(num_measurements, measurements);
    kf.batch_filter(num_measurements, measurements);
    return 0;
}
