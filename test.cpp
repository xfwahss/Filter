#include "KalmanFilter.h"
#include <iostream>

int main() {
    // 定义系统参数
    Eigen::VectorXd x0(1); // 初始状态
    Eigen::MatrixXd P0(1, 1); // 初始状态协方差矩阵
    Eigen::MatrixXd A(1, 1); // 状态转移矩阵
    Eigen::MatrixXd H(1, 1); // 观测矩阵
    Eigen::MatrixXd Q(1, 1); // 过程噪声协方差矩阵
    Eigen::MatrixXd R(1, 1); // 测量噪声协方差矩阵

    // 初始化系统参数
    x0 << 0; // 初始位置
    P0 << 1; // 初始状态协方差
    A << 1;  // 状态转移为恒等矩阵
    H << 1;  // 观测矩阵为恒等矩阵
    Q << 0.1; // 过程噪声较小
    R << 1;   // 测量噪声较大

    // 创建卡尔曼滤波器对象
    KalmanFilter kf;
    kf.initialize(x0, P0, A, H, Q, R);

    // 模拟一些测量
    double measurements[] = {1.1, 2.0, 3.1, 4.0, 5.2};
    size_t num_measurements = sizeof(measurements) / sizeof(measurements[0]);

    // 使用卡尔曼滤波器进行状态估计
    for (size_t i = 0; i < num_measurements; ++i) {
        // 预测
        kf.predict();

        // 更新
        const Eigen::VectorXd measurement(measurements[i]);
        kf.update(measurement);

        // 获取估计的位置
        Eigen::VectorXd estimated_state = kf.getState();

        // 输出估计的位置
        std::cout << "Estimated Position: " << estimated_state(0) << std::endl;
    }

    return 0;
}
