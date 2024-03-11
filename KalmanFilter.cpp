#include "KalmanFilter.h"

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::initialize(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0,
                              const Eigen::MatrixXd& A, const Eigen::MatrixXd& H,
                              const Eigen::MatrixXd& Q, const Eigen::MatrixXd& R) {
    state_ = x0;
    P_ = P0;
    A_ = A;
    H_ = H;
    Q_ = Q;
    R_ = R;
}

void KalmanFilter::predict() {
    // 预测步骤
    state_ = A_ * state_;
    P_ = A_ * P_ * A_.transpose() + Q_;
}

void KalmanFilter::update(const Eigen::VectorXd& measurement) {
    // 更新步骤
    Eigen::MatrixXd K = P_ * H_.transpose() * (H_ * P_ * H_.transpose() + R_).inverse();
    state_ = state_ + K * (measurement - H_ * state_);
    P_ = (Eigen::MatrixXd::Identity(P_.rows(), P_.cols()) - K * H_) * P_;
}

Eigen::VectorXd KalmanFilter::getState() const {
    return state_;
}