#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include "../../io/include/FilterIO.h"
#include "FilterBase.h"
#include <Eigen/Dense>
#include <random>
#include <vector>

// 集合卡尔曼滤波模板类，只能实现高斯分布采样
// 模拟类需要实现 update(接收滤波器更新了的参数)， predict(向前推进)方法,
// 和获取当前状态的方法 预测设定为精确预测吧，不确定度由集合卡尔曼滤波类实现
// 应该是每融合一次都需要重新采样, 这样才能确保集合的分布是融合过后的

template <class T> class EnsembleKalmanFilter : public FilterBase {
  public:
    EnsembleKalmanFilter(const int &ensemble_size);
    ~EnsembleKalmanFilter();
    void init(Eigen::VectorXd X, Eigen::MatrixXd P, Eigen::MatrixXd H,
              Eigen::VectorXd Q);

    void step_assimilation(const double &dt, const Eigen::VectorXd &z,
                           const Eigen::MatrixXd &R);
    /*
    每个状态每次只有一个观测值,多个观测值给定R的另写
    * 改成多个文件协同
    */
    void batch_assimilation(FilterIO *modelio, const double &dt,
                            const Eigen::MatrixXd &R = Eigen::MatrixXd());
    Eigen::VectorXd get_status();
    Eigen::MatrixXd get_covariance();

  private:
    std::vector<T *> ensemble;
    // 添加过程预测的误差，超参数，人为给定
    // 采用单个推进，误差为向量形式,给定每个方差的大小，均值为0，使用正态随机数
    Eigen::VectorXd Q;
    // 观测矩阵H
    Eigen::MatrixXd H;

    /* 处理具有多组观测数据的均值和协方差，存储格式为, m * n
       矩阵，有m个观测变量，每个观测变量有n个值
        @param z 存储观测均值
        @param R 存储观测的协方差矩阵
    */
    void process_measurements(Eigen::MatrixXd mesurements, Eigen::VectorXd &z,
                              Eigen::MatrixXd &R);

    // 根据给定的方差生成一个状态的预测随机误差
    Eigen::VectorXd generate_process_error();

    void ensemble_construct(const int &ensemble_size);
    Eigen::VectorXd multivariate_gaussian_random(Eigen::VectorXd &status,
                                                 Eigen::MatrixXd &covariance);
    void sample();
    void sample_predict(const double &dt);
    void ensemble_destruct();

    void predict(const double &dt);
    void update(Eigen::VectorXd z, Eigen::MatrixXd R);
};

template <class T>
EnsembleKalmanFilter<T>::EnsembleKalmanFilter(const int &ensemble_size) {
    ensemble.reserve(ensemble_size);
    ensemble_construct(ensemble_size);
}

template <class T>
void EnsembleKalmanFilter<T>::ensemble_construct(const int &ensemble_size) {
    for (int i = 0; i < ensemble_size; i++) {
        T *ptr = new T();
        ensemble.push_back(ptr);
    }
}

template <class T>
void EnsembleKalmanFilter<T>::init(Eigen::VectorXd X, Eigen::MatrixXd P,
                                   Eigen::MatrixXd H, Eigen::VectorXd Q) {
    FilterBase::X = X;
    FilterBase::P = P;
    this->H       = H;
    this->Q       = Q;
}

template <class T> EnsembleKalmanFilter<T>::~EnsembleKalmanFilter() {
    ensemble_destruct();
}

template <class T> void EnsembleKalmanFilter<T>::predict(const double &dt) {
    sample();
    sample_predict(dt);

    // 计算均值向量
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(FilterBase::X.rows());
    for (int i = 0; i < ensemble.size(); ++i) {
        mean += ensemble[i]->get_status();
    }
    mean /= ensemble.size();

    // 计算协方差矩阵
    Eigen::MatrixXd cov =
        Eigen::MatrixXd::Zero(FilterBase::P.rows(), FilterBase::P.cols());
    for (int j = 0; j < ensemble.size(); ++j) {
        // 需要考虑过程预测引起的误差,添加了get_process_error
        Eigen::VectorXd diff =
            ensemble[j]->get_status() + generate_process_error() - mean;
        cov += diff * diff.transpose();
    }
    cov /= ensemble.size() - 1;
    FilterBase::X = mean;
    FilterBase::P = cov;
}

template <class T>
void EnsembleKalmanFilter<T>::process_measurements(Eigen::MatrixXd measurements,
                                                   Eigen::VectorXd &z,
                                                   Eigen::MatrixXd &R) {
    int var_nums         = measurements.rows();
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(var_nums);
    Eigen::MatrixXd cov  = Eigen::MatrixXd::Zero(var_nums, var_nums);

    for (int i = 0; i < measurements.cols(); i++) {
        mean += measurements.col(i);
    }
    mean /= measurements.cols();
    for (int i = 0; i < measurements.cols(); ++i) {
        Eigen::VectorXd diff = measurements.col(i) - mean;
        cov += diff * diff.transpose();
    }
    cov /= measurements.cols() - 1;
    z = mean;
    R = cov;
}

template <class T>
void EnsembleKalmanFilter<T>::update(Eigen::VectorXd z, Eigen::MatrixXd R) {
    Eigen::MatrixXd K =
        P * H.transpose() * (H * P * H.transpose() + R).inverse();
    X = X + K * (z - H * X);
    P = (Eigen::MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P;
}

template <class T>
void EnsembleKalmanFilter<T>::step_assimilation(const double &dt,
                                                const Eigen::VectorXd &z,
                                                const Eigen::MatrixXd &R) {
    predict(dt);
    update(z, R);
}

template <class T>
Eigen::VectorXd EnsembleKalmanFilter<T>::multivariate_gaussian_random(
    Eigen::VectorXd &status, Eigen::MatrixXd &covariance) {
    std::random_device seed;
    std::mt19937_64 engine(seed());
    std::normal_distribution<double> dist(0.0, 1.0);

    // 使用Eigen库计算Cholesky分解
    Eigen::LLT<Eigen::MatrixXd> lltOfCov(covariance);
    Eigen::MatrixXd L = lltOfCov.matrixL();

    // 生成随机数
    Eigen::VectorXd z(status.size());
    for (int i = 0; i < status.size(); ++i) {
        z(i) = dist(engine);
    }
    Eigen::VectorXd random_number = status + L * z;
    return random_number;
}

template <class T> void EnsembleKalmanFilter<T>::sample() {
    for (int i = 0; i < ensemble.size(); ++i) {
        Eigen::VectorXd random_num =
            multivariate_gaussian_random(FilterBase::X, FilterBase::P);
        ensemble[i]->update(random_num);
    }
}

template <class T>
void EnsembleKalmanFilter<T>::sample_predict(const double &dt) {
    for (int i = 0; i < ensemble.size(); ++i) {
        ensemble[i]->step(dt);
    }
}

template <class T> void EnsembleKalmanFilter<T>::ensemble_destruct() {
    for (int i = 0; i < ensemble.size(); i++) {
        delete ensemble[i];
    }
    ensemble.clear();
}

template <class T>
Eigen::VectorXd EnsembleKalmanFilter<T>::generate_process_error() {
    Eigen::VectorXd error(FilterBase::X.rows());
    for (int i = 0; i < FilterBase::X.rows(); ++i) {
        std::random_device seed;
        std::mt19937_64 engine(seed());
        std::normal_distribution<double> dist(0.0, Q(i));
        error(i) = dist(engine);
    }
    return error;
}

template <class T> Eigen::VectorXd EnsembleKalmanFilter<T>::get_status() {
    return FilterBase::X;
}

template <class T> Eigen::MatrixXd EnsembleKalmanFilter<T>::get_covariance() {
    return FilterBase::P;
}

template <class T>
void EnsembleKalmanFilter<T>::batch_assimilation(FilterIO *filterio,
                                                 const double &dt,
                                                 const Eigen::MatrixXd &R) {
    Eigen::VectorXd z;
    Eigen::MatrixXd R_file;
    filterio->write_headers();
    if (R.size() == 0) {
        filterio->get_obs(z, R_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, R_file);
            filterio->write_x(X);
            filterio->write_P(P);
            filterio->get_obs(z, R_file);
        }
    } else {
        filterio->get_obs(z, R_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, R);
            filterio->write_x(X);
            filterio->write_P(P);
            filterio->get_obs(z, R_file);
        }
    }
}

// 提供抽象类提供作为集合卡尔曼滤波的模板类基类
class EnsembleModel {
  private:
    Eigen::VectorXd status;

  protected:
    // 状态不变更新
    Eigen::VectorXd constant_predict(const Eigen::VectorXd &s,
                                     const double &dt) {
        return s;
    }
    /* @brief 假设一阶导数不变的参数更新
     * @param s VectorXd的数据存储方式应该为 [状态， 1阶导数]
     * @param dt 预测时间间隔
     */
    Eigen::Vector2d const_1derivate_predict(const Eigen::Vector2d &s,
                                            const double &dt) {
        Eigen::Vector2d next;
        next(1) = s(1);
        next(0) = s(0) + s(1) * dt;
        return next;
    }
    // 二阶导数不变更新
    /* @brief 假设二阶导数不变的参数更新
    * @param s Vector3d的存储格式为 [状态, 一阶导数, 二阶导数]
    * @param dt 预测时间间隔
    */
    Eigen::Vector3d const_2derivate_predict(const Eigen::Vector3d &s,
                                            const double &dt) {
        Eigen::Vector3d next;
        next(2) = s(2);
        next(1) = s(1) + s(2) * dt;
        next(0) = s(0) + s(1) * dt + 0.5 * s(2) * dt * dt;
        return s;
    }

  public:
    Eigen::VectorXd get_status() { return status; }
    void update(Eigen::VectorXd &status) { this->status = status; }
    // 子类必须实现predict的方法
    virtual Eigen::VectorXd predict(const double &dt) = 0;
    void step(const double &dt){
        Eigen::VectorXd predicted_status = predict(dt);
        update(predicted_status);
    }
};

#endif
