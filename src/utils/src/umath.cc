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

double umath::std_normal(const unsigned int &seed) {
    static std::mt19937_64 engine(seed == 0 ? std::random_device()() : seed);
    static std::normal_distribution<double> dist(0, 1);
    static bool initialized = false;
    if (!initialized) {
        Logger::get("umath")->info("Init random engine with seed {}", seed);
        initialized = true;
    }
    double value = dist(engine);
    return value;
}

double umath::randomd(const double &mean, const double &variance, const unsigned int &seed) {
    double stddev = std::sqrt(variance);
    double value  = umath::std_normal(seed) * stddev + mean;
    return value;
}

// 在有误差的情况下计算相对误差
Eigen::VectorXd umath::re(const Eigen::VectorXd &u1, const Eigen::MatrixXd &cov1, const Eigen::VectorXd &u2,
                          const Eigen::MatrixXd &cov2, const int &sample_size, const unsigned int &seed) {
    Logger::log_vectorxd("u1: {}", u1, "umath");
    Logger::log_vectorxd("u2: {}", u2, "umath");
    Eigen::VectorXd re = Eigen::VectorXd::Zero(u1.size());
    for (int i = 0; i < re.size(); ++i) {
        double re_sum = 0;
        for (int j = 0; j < sample_size; ++j) {
            double sample_u1  = randomd(u1(i), cov1(i, i), seed);
            double sample_u2  = randomd(u2(i), cov2(i, i), seed);
            double single_re  = -0.5 * sample_u2 / sample_u1 + 0.5 * sample_u1 / sample_u2;
            re_sum           += single_re;
        }
        Logger::get("umath")->debug("re_sum: {}", re_sum);
        re(i) = re_sum / (double)sample_size;
    }
    return re;
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
        z(i) = umath::std_normal(seed);
    }
    Eigen::VectorXd random_number = mean + L * z;
    return random_number;
}

Eigen::VectorXd umath::pos_multi_gauss_random(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance,
                                              const unsigned int &seed) {
    int count                     = 0;
    Eigen::VectorXd random_vector = umath::multivariate_gaussian_random(mean, covariance, seed);
    Logger::get("console")->enable_backtrace(5);
    while ((!is_positive(random_vector)) && count < 100) {
        random_vector = umath::multivariate_gaussian_random(mean, covariance, seed);
        Logger::log_vectorxd("random vector:{}", random_vector);
        count++;
    }
    if (count == 100) {
        Logger::get()->warn("Negative random vector was generated!");
        Logger::get()->dump_backtrace();
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