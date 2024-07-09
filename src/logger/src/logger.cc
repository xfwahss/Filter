#include <logger.h>

void Logger::infoEigenVector(const std::string &msg,
                             const Eigen::VectorXd &vec) {
    std::stringstream ss;
    ss << vec.transpose(); // 将向量以行的形式输出到stringstream
    info(msg, ss.str());   // 调用spdlog::logger的info方法输出
}

void Logger::infoEigenMatrix(const std::string &msg,
                             const Eigen::MatrixXd &mat) {
    std::stringstream ss;
    ss << mat;
    info(msg, ss.str());
}