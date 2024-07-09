#ifndef LOGGER_H
#define LOGGER_H
#include "spdlog/spdlog.h"
#include <Eigen/Dense>

// 创建spdlog的子类  
class EigenSpdLog : public spdlog::logger {  
public:  
    // 使用spdlog::logger的构造函数  
    using spdlog::logger::logger;  
  
    // 封装输出Eigen::VectorXd的方法  
    void infoEigenVector(const std::string& msg, const Eigen::VectorXd& vec) {  
        std::stringstream ss;  
        ss << vec.transpose();  // 将向量以行的形式输出到stringstream  
        info(msg, ss.str());    // 调用spdlog::logger的info方法输出  
    }  
  
    // 封装输出Eigen::MatrixXd的方法  
    void infoEigenMatrix(const std::string& msg, const Eigen::MatrixXd& mat) {  
        std::stringstream ss;  
        ss << mat;
        info(msg, ss.str());
    }  
};  

int main() {  
    // 初始化spdlog  
    auto console = std::make_shared<EigenSpdLog>("console", spdlog::level::info);  
    spdlog::register_logger(console);  
  
    // 创建一个Eigen::VectorXd对象  
    Eigen::VectorXd vec(5);  
    vec << 1.0, 2.0, 3.0, 4.0, 5.0;  
  
    // 使用封装的方法输出Eigen::VectorXd  
    console->infoEigenVector("Eigen::VectorXd的内容: ", vec);  
  
    // 创建一个Eigen::MatrixXd对象  
    Eigen::MatrixXd mat(2, 3);  
    mat << 1.0, 2.0, 3.0,  
         4.0, 5.0, 6.0;  
  
    // 使用封装的方法输出Eigen::MatrixXd  
    console->infoEigenMatrix("Eigen::MatrixXd的内容: ", mat);  
  
    return 0;  
}

#endif