#include "KalmanFilter.h"
#include "MatFileIO.h"
#include <Eigen/Dense>
#include <iostream>

void printhelp() { std::cout << "Usage: test -f <filename> [-h]" << std::endl; }

void filter(const char *filename) {
    // Eigen::VectorXd x(1); // 初始状态
    // Eigen::VectorXd u(1); // 初始状态
    // Eigen::MatrixXd P(1, 1); // 初始状态协方差矩阵
    // Eigen::MatrixXd B(1, 1); // 初始状态协方差矩阵
    // Eigen::MatrixXd F(1, 1); // 状态转移矩阵
    // Eigen::MatrixXd H(1, 1); // 观测矩阵
    // Eigen::MatrixXd Q(1, 1); // 过程噪声协方差矩阵
    // Eigen::MatrixXd R(1, 1); // 测量噪声协方差矩阵
    MatFileIO matio("../../src/test/init_matrix.txt");
    matio.read();
    InitState ins = matio.get_initstate();

    // // 初始化系统参数
    // x << 0.5; // 初始位置
    // P << 10; // 初始状态协方差
    // F << 1;  // 状态转移为恒等矩阵
    // H << 1;  // 观测矩阵为恒等矩阵
    // Q << 0.01; // 过程噪声较小
    // R << 1;   // 测量噪声较大
    // B << 0;
    // u << 0;
    Eigen::VectorXd x = ins.get_vector("x");
    Eigen::VectorXd u = ins.get_vector("u");
    Eigen::MatrixXd P = ins.get_matrix("P");
    Eigen::MatrixXd F = ins.get_matrix("F");
    Eigen::MatrixXd B = ins.get_matrix("B");
    Eigen::MatrixXd H = ins.get_matrix("H");
    Eigen::MatrixXd R = ins.get_matrix("R");
    Eigen::MatrixXd Q = ins.get_matrix("Q");
    std::cout << x << std::endl;

    KalmanFilter kf(2, 1);
    kf.init(x, P, F, Q, B, u, H, R);

    FilterIO io(filename);

    // double measurements[] =
    // {2.1, 2.0, 2.1, 2.5, 2.2, 2.3, 3.5, 2.7, 2.8, 2.4, 2.3}; size_t
    // num_measurements = sizeof(measurements) / sizeof(measurements[0]);
    kf.batch_filter(io);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No arguments provided. Use -h for help."
                  << std::endl;
        return 1;
    }
    if (strcmp(argv[1], "-h") == 0) {
        printhelp();
    } else if (strcmp(argv[1], "-f") == 0 && argc >= 3) {
        std::cout << "File specified: " << argv[2] << std::endl;
        filter(argv[2]);
        return 0;
    } else {
        std::cerr << "Error: Invalid arguments. Use -h for help." << std::endl;
        return 1;
    }
}
