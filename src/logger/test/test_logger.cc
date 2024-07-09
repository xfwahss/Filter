#include<logger.h>
#include<iostream>
int main() {  
    // 初始化spdlog  
    // auto console = std::make_shared<Logger>("console", spdlog::level::info);  
    // spdlog::register_logger(console);  
  
    // // 创建一个Eigen::VectorXd对象  
    // Eigen::VectorXd vec(5);  
    // vec << 1.0, 2.0, 3.0, 4.0, 5.0;  
  
    // // 使用封装的方法输出Eigen::VectorXd  
    // console->infoEigenVector("Eigen::VectorXd的内容: ", vec);  
  
    // // 创建一个Eigen::MatrixXd对象  
    // Eigen::MatrixXd mat(2, 3);  
    // mat << 1.0, 2.0, 3.0,  
    //      4.0, 5.0, 6.0;  
  
    // // 使用封装的方法输出Eigen::MatrixXd  
    // console->infoEigenMatrix("Eigen::MatrixXd的内容: ", mat);  
    std::cout<< "Hello" << std::endl;
  
    return 0;  
}