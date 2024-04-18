#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include<Eigen/Dense>
#include <vector>

// 集合卡尔曼滤波模板类，只能实现高斯分布采样
// 模拟类需要实现 init， update， predict方法, 预测设定为精确预测吧，不确定度由集合卡尔曼滤波类实现
// 应该是每融合一次都需要重新采样, 这样才能确保集合的分布是融合过后的

template <class T> class EnsembleKalmanFilter {
  private:
    std::vector<T *> ensemble;
    void batch_construct(const int &ensemble_size);
    // 先归一化再采样,使得采样尽可能均匀
    void batch_sample(Eigen::VectorXd &status, Eigen::MatrixXd &covariance);
    void batch_destruct();

  public:
    EnsembleKalmanFilter(const int &ensemble_size);
    ~EnsembleKalmanFilter();
    void predict();
    void update();
};

template <class T>
EnsembleKalmanFilter<T>::EnsembleKalmanFilter(const int &ensemble_size) {
    ensemble.reserve(ensemble_size);
    batch_construct();
}
template <class T> EnsembleKalmanFilter<T>::~EnsembleKalmanFilter() {
    batch_destruct();
}
template <class T>
void EnsembleKalmanFilter<T>::batch_construct(const int &ensemble_size) {
    for (int i = 0; i < ensemble_size; i++) {
        T *ptr = new T();
        ensemble.push_back(ptr);
    }
}

template <class T> void EnsembleKalmanFilter<T>::batch_destruct() {
    for (int i = 0; i < ensemble.size(); i++) {
        delete ensemble[i];
    }
    ensemble.clear();
}

#endif