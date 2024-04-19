#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include "FilterBase.h"
#include <Eigen/Dense>
#include <random>
#include <vector>

#include <iostream>
// 集合卡尔曼滤波模板类，只能实现高斯分布采样
// 模拟类需要实现 init， update， predict方法,
// 预测设定为精确预测吧，不确定度由集合卡尔曼滤波类实现
// 应该是每融合一次都需要重新采样, 这样才能确保集合的分布是融合过后的

template <class T> class EnsembleKalmanFilter : public FilterBase {
  public:
    EnsembleKalmanFilter(const int &ensemble_size, Eigen::VectorXd X,
                         Eigen::MatrixXd P);
    ~EnsembleKalmanFilter();
    void predict(const double &dt);
    void update();
    Eigen::VectorXd current_status();
    Eigen::MatrixXd current_covariance();

  private:
    std::vector<T *> ensemble;
    void batch_construct(const int &ensemble_size);
    // 先归一化再采样,使得采样尽可能均匀
    Eigen::VectorXd multivariate_gaussian_random(Eigen::VectorXd &status,
                                                 Eigen::MatrixXd &covariance);
    void batch_sample();
    void batch_predict(const double &dt);
    void batch_destruct();
};

template <class T>
EnsembleKalmanFilter<T>::EnsembleKalmanFilter(const int &ensemble_size,
                                              Eigen::VectorXd X,
                                              Eigen::MatrixXd P) {
    FilterBase::X = X;
    FilterBase::P = P;
    ensemble.reserve(ensemble_size);
    batch_construct(ensemble_size);
}
template <class T> EnsembleKalmanFilter<T>::~EnsembleKalmanFilter() {
    batch_destruct();
}

template <class T> void EnsembleKalmanFilter<T>::predict(const double &dt) {
    batch_sample();
    batch_predict(dt);

    // 计算均值向量
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(FilterBase::X.rows());
    for (int i = 0; i < ensemble.size(); ++i) {
        mean += ensemble[i]->current_status();
    }
    mean /= ensemble.size();

    // 计算协方差矩阵
    Eigen::MatrixXd cov =
        Eigen::MatrixXd::Zero(FilterBase::P.rows(), FilterBase::P.cols());
    for (int j = 0; j < ensemble.size(); ++j) {
        Eigen::VectorXd diff = ensemble[j]->current_status() - mean;
        cov += diff * diff.transpose();
    }
    cov /= ensemble.size() - 1;
    FilterBase::X = mean;
    FilterBase::P = cov;
}

template <class T> void EnsembleKalmanFilter<T>::update() {}

template <class T>
void EnsembleKalmanFilter<T>::batch_construct(const int &ensemble_size) {
    for (int i = 0; i < ensemble_size; i++) {
        T *ptr = new T();
        ensemble.push_back(ptr);
    }
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

template <class T> void EnsembleKalmanFilter<T>::batch_sample() {
    // 统一生成方法，空间换时间
    // Eigen::MatrixXd random_num(FilterBase::X.rows(), ensemble.size());
    // for (int i = 0; i < FilterBase::X.rows(); ++i) {
    //     std::random_device seed;
    //     std::mt19937_64 engine(seed());
    //     std::normal_distribution dist(0.0, 1.0);
    //     for (int j = 0; j < ensemble.size(); ++j) {
    //         random_num.row(i)[j] = dist(engine);
    //     }
    // }

    // Eigen::LLT<Eigen::MatrixXd> lltOfCov(FilterBase::P);
    // Eigen::MatrixXd L = lltOfCov.matrixL();
    // std::cout<< L * L.transpose() << std::endl;

    // for(int i=0; i< ensemble.size(); ++i){
    //     Eigen::VectorXd random_number = FilterBase::X + L * random_num.col(i);
    //     ensemble[i] -> update_status(random_number);
    // }


    for (int i = 0; i < ensemble.size(); ++i) {
        Eigen::VectorXd random_num =
            multivariate_gaussian_random(FilterBase::X, FilterBase::P);
        ensemble[i]->update_status(random_num);
    }
}

// TODO::应该添加预测误差
template <class T>
void EnsembleKalmanFilter<T>::batch_predict(const double &dt) {
    for (int i = 0; i < ensemble.size(); ++i) {
        ensemble[i]->predict(dt);
    }
}

template <class T> void EnsembleKalmanFilter<T>::batch_destruct() {
    for (int i = 0; i < ensemble.size(); i++) {
        delete ensemble[i];
    }
    ensemble.clear();
}

template <class T> Eigen::VectorXd EnsembleKalmanFilter<T>::current_status() {
    return FilterBase::X;
}

template <class T>
Eigen::MatrixXd EnsembleKalmanFilter<T>::current_covariance() {
    return FilterBase::P;
}
#endif
