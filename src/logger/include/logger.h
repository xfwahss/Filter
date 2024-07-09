#ifndef LOGGER_H
#define LOGGER_H
#include <spdlog/spdlog.h>
#include <Eigen/Dense>

// 创建spdlog的子类
class Logger : public spdlog::logger {
  public:
    // 使用spdlog::logger的构造函数
    using spdlog::logger::logger;

    // 封装输出Eigen::VectorXd的方法
    void infoEigenVector(const std::string &msg, const Eigen::VectorXd &vec);

    // 封装输出Eigen::MatrixXd的方法
    void infoEigenMatrix(const std::string &msg, const Eigen::MatrixXd &mat);
};

#endif