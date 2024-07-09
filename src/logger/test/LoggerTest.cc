#include <logger.h>
#include <stdexcept>
int main() {
    // 初始化spdlog
    // // 创建一个Eigen::VectorXd对象
    Eigen::VectorXd vec(5);
    vec << 1.0, 2.0, 3.0, 4.0, 5.0;
    // logger::get()->set_level(spdlog::level::debug);
    logger::get("console")->info("This is the first");
    logger::get("console")->info("This is the second");

    logger::get("console")->enable_backtrace(32);

    // // 创建一个Eigen::MatrixXd对象
    Eigen::MatrixXd mat(2, 3);
    mat << 1.0, 2.0, 3.0,
         4.0, 5.0, 6.0;

    // 使用封装的方法输出Eigen::MatrixXd
    logger::log_matrixxd("Eigen::MatrixXd:{}", mat);
    try {
        for (int i = 0; i < 100; i++) {

            if(i==40){
                throw std::runtime_error("Hello");
            }
            int a = i / (i - 40);
            logger::get("console")->debug("message: {}", i);
            logger::log_vectorxd("Teshi:{}", vec);
        }
    } catch (...) {
        logger::get("console")->dump_backtrace();
    }
    // // 使用封装的方法输出Eigen::VectorXd
    logger::log_vectorxd("Eigen::VectorXd:{}",  vec);


    return 0;
}