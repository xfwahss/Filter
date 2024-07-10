#include <random>
#include <umath.h>


bool umath::is_positive(const Eigen::VectorXd &value) {
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        if (value(i) < 0) {
            return false;
        }
    }
    return true;
}

double umath::randomd(const double &mean, const double &variance, const unsigned int &seed) {
    static std::mt19937_64 *engine = nullptr;
    double stddev                  = std::sqrt(variance);
    // 如果随机数生成器尚未初始化，则进行初始化
    if (engine == nullptr) {
        // 如果seed为0，则使用随机设备生成种子
        unsigned int actual_seed = (seed == 0) ? std::random_device()() : seed;
        engine                   = new std::mt19937_64(actual_seed);
        logger::get()->info("Init random engine with seed {}.", actual_seed);
    }

    std::normal_distribution<double> dist(mean, stddev);
    // 使用随机数生成器和分布生成随机数
    double value = dist(*engine);
    // 注意：这里没有删除engine，因为它们在函数的生命周期内是持久的
    // 如果需要在程序结束时清理这些资源，需要在其他地方管理它们（例如，使用智能指针或析构函数）
    // 但请注意，这通常不是必需的，因为当程序结束时，所有静态局部变量的内存都将被自动释放
    return value;
}

double umath::fill_missed_value(const double &value, const double &missed_value, const double &fill_value) {
    if (value == missed_value) {
        return fill_value;
    } else {
        return value;
    }
}

Eigen::VectorXd umath::multivariate_gaussian_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance,
                                                    const unsigned int &seed) {
    // 使用Eigen库计算Cholesky分解
    Eigen::LLT<Eigen::MatrixXd> lltOfCov(covariance);
    Eigen::MatrixXd L = lltOfCov.matrixL();

    Eigen::VectorXd z(mean.size());
    for (int i = 0; i < mean.size(); ++i) {
        z(i) = umath::randomd(0, 1, seed);
    }
    Eigen::VectorXd random_number = mean + L * z;
    return random_number;
}

Eigen::VectorXd umath::pos_multi_gauss_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance,
                                              const unsigned int &seed) {
    int count = 0;
    Eigen::VectorXd random_vector = umath::multivariate_gaussian_random(mean, covariance, seed);
    logger::get("console")->enable_backtrace(5);
    while ((!is_positive(random_vector)) && count < 100) {
        random_vector = umath::multivariate_gaussian_random(mean, covariance, seed);
        logger::log_vectorxd("random vector:{}", random_vector);
        count++;
    }
    if(count==100){
        logger::get()->warn("Negative random vector was generated!");
        logger::get()->dump_backtrace();
    }

    return random_vector;
}

Eigen::MatrixXd umath::covariance(const Eigen::MatrixXd &mat) {
    /* mat的数据存储格式
     * 矩阵的列数为变量的个数
     *矩阵的行数为每个变量的观测值个数
     */
    Eigen::MatrixXd covariance = Eigen::MatrixXd::Zero(mat.cols(), mat.cols());
    Eigen::VectorXd sum        = Eigen::VectorXd::Zero(mat.cols());
    for (int i = 0; i < mat.cols(); ++i) {
        for (int j = 0; j < mat.rows(); ++j) {
            sum(i) += mat(j, i);
        }
    }
    Eigen::VectorXd mean = sum / mat.rows();
    for (int i = 0; i < mat.rows(); ++i) {
        covariance += (mat.row(i).transpose() - mean) * (mat.row(i).transpose() - mean).transpose();
    }
    return covariance / (mat.rows() - 1);
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