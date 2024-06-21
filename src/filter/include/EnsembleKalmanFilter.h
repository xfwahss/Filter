/*!
 * @file EnsembleKalmanFilter.h
 * @brief 集合卡尔曼滤波模板
 * 1. 采用高斯分布采样
 * 2. 模板类需要实现update(VectorXd)方法和predict()方法以及获取当前状态的方法，
 *    预测过程使用模型的精确预测，不确定度由集合卡尔曼滤波给定
 * 3. 每次分析步过后都会重新采样, 确保采样后的集合均值和协方差与给定的一致
 * @author xfwahss
 * @version main-v0.2
 * @date 2024-06-04
 */
#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include "../../io/include/FilterIO.h"
#include "../../utils/include/umath.h"
#include "FilterBase.h"
#include <Eigen/Dense>
#include <random>
#include <vector>

template <class T> class EnsembleKalmanFilter : public FilterBase {
  public:
    EnsembleKalmanFilter(const int &ensemble_size);
    ~EnsembleKalmanFilter();

    void init(Eigen::VectorXd X, Eigen::MatrixXd P, Eigen::MatrixXd H,
              Eigen::VectorXd Q);

    // 单步同化
    void step_assimilation(const double &dt, const Eigen::VectorXd &z,
                           const Eigen::MatrixXd &R, bool ensure_pos);
    // 批量同化
    /*
     * @param ensure_pos 是否保证采样的值全为正数
     */
    void batch_assimilation(FilterIO *filterio, const double &dt,
                            const Eigen::MatrixXd &R = Eigen::MatrixXd(),
                            bool ensure_pos          = false);
    Eigen::VectorXd status();
    Eigen::MatrixXd covariance();

  private:
    // 存储指向集合中子模型的指针
    std::vector<T *> ensemble;
    // 采用单个推进，误差为向量形式,给定每个方差的大小，均值为0，使用正态随机数
    Eigen::VectorXd Q;
    // 观测矩阵H
    Eigen::MatrixXd H;
    // 根据给定的方差生成一个状态的预测随机误差
    Eigen::VectorXd generate_process_error();

    // 构建集合
    void construct(const int &ensemble_size);
    // 销毁集合
    void destruct();

    // 状态采样
    void sample(bool ensure_pos);
    // 样本状态预测
    void sample_predict(const double &dt);

    // 利用样本预测估计整体
    void predict(const double &dt, bool ensure_pos);
    void update(Eigen::VectorXd z, Eigen::MatrixXd R);
};

template <class T>
EnsembleKalmanFilter<T>::EnsembleKalmanFilter(const int &ensemble_size) {
    ensemble.reserve(ensemble_size);
    construct(ensemble_size);
}

template <class T>
void EnsembleKalmanFilter<T>::step_assimilation(const double &dt,
                                                const Eigen::VectorXd &z,
                                                const Eigen::MatrixXd &R,
                                                bool ensure_pos) {
    predict(dt, ensure_pos);
    update(z, R);
}

template <class T>
void EnsembleKalmanFilter<T>::batch_assimilation(FilterIO *filterio,
                                                 const double &dt,
                                                 const Eigen::MatrixXd &R,
                                                 bool ensure_pos) {
    Eigen::VectorXd z;
    Eigen::MatrixXd r_file;
    filterio->write_headers();
    if (R.size() == 0) {
        filterio->read(z, r_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, r_file, ensure_pos);
            filterio->write_x(X, "X", 1, 2);
            filterio->write_P(P, "P", 1, 2);
            filterio->read(z, r_file);
        }
    } else {
        filterio->read(z, r_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, R, ensure_pos);
            filterio->write_x(X, "X", 1, 2);
            filterio->write_P(P, "P", 1, 2);
            filterio->read(z, r_file);
        }
    }
}

template <class T>
void EnsembleKalmanFilter<T>::construct(const int &ensemble_size) {
    for (int i = 0; i < ensemble_size; i++) {
        T *ptr = new T();
        ensemble.push_back(ptr);
    }
}

template <class T> void EnsembleKalmanFilter<T>::destruct() {
    for (int i = 0; i < ensemble.size(); i++) {
        delete ensemble[i];
    }
    ensemble.clear();
}

template <class T> void EnsembleKalmanFilter<T>::sample(bool ensure_pos) {
    for (int i = 0; i < ensemble.size(); ++i) {
        Eigen::VectorXd random_num;
        if (ensure_pos) {
            random_num = umath::pos_multi_gauss_random(X, P);
        } else {
            random_num = umath::multivariate_gaussian_random(X, P);
        }
        ensemble[i]->update(random_num);
    }
}

template <class T>
void EnsembleKalmanFilter<T>::sample_predict(const double &dt) {
    for (int i = 0; i < ensemble.size(); ++i) {
        ensemble[i]->step(dt);
    }
}

template <class T>
void EnsembleKalmanFilter<T>::predict(const double &dt, bool ensure_pos) {
    sample(ensure_pos);
    sample_predict(dt);

    // 计算均值向量
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(X.rows());
    for (int i = 0; i < ensemble.size(); ++i) {
        mean += ensemble[i]->get_status();
    }
    mean                /= ensemble.size();

    // 计算协方差矩阵
    Eigen::MatrixXd cov  = Eigen::MatrixXd::Zero(P.rows(), P.cols());
    for (int j = 0; j < ensemble.size(); ++j) {
        // 需要考虑过程预测引起的误差,添加了get_process_error
        Eigen::VectorXd diff =
            ensemble[j]->get_status() + generate_process_error() - mean;
        cov += diff * diff.transpose();
    }
    cov /= ensemble.size() - 1;
    X    = mean;
    P    = cov;
}

template <class T>
void EnsembleKalmanFilter<T>::update(Eigen::VectorXd z, Eigen::MatrixXd R) {
    Eigen::MatrixXd K;
    K = P * H.transpose() * (H * P * H.transpose() + R).inverse();
    X = X + K * (z - H * X);
    P = (Eigen::MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P;
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
    destruct();
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

template <class T> Eigen::VectorXd EnsembleKalmanFilter<T>::status() {
    return FilterBase::X;
}

template <class T> Eigen::MatrixXd EnsembleKalmanFilter<T>::covariance() {
    return FilterBase::P;
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
        return next;
    }

  public:
    Eigen::VectorXd get_status() { return status; }
    void update(Eigen::VectorXd &status) { this->status = status; }
    // 子类必须实现predict的方法
    virtual Eigen::VectorXd predict(const double &dt,
                                    const Eigen::VectorXd &status) = 0;
    void step(const double &dt) {
        Eigen::VectorXd predicted_status = predict(dt, status);
        update(predicted_status);
    }
};

class EnsembleIO : public FilterIO {};

#endif
