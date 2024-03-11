#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

#include <Eigen/Dense>

class KalmanFilter {
public:
    KalmanFilter();
    ~KalmanFilter();

    // Init the kalman filter
    void initialize(const Eigen::VectorXd& x0, const Eigen::MatrixXd& P0, 
                    const Eigen::MatrixXd& A, const Eigen::MatrixXd& H,
                    const Eigen::MatrixXd& Q, const Eigen::MatrixXd& R);

    // predict
    void predict();

    // update
    void update(const Eigen::VectorXd& measurement);

    // get current state
    Eigen::VectorXd getState() const;

private:
    // state vector
    Eigen::VectorXd state_;

    // 状态协方差矩阵
    Eigen::MatrixXd P_;

    // 状态转移矩阵
    Eigen::MatrixXd A_;

    // 观测矩阵
    Eigen::MatrixXd H_;

    // 过程噪声协方差矩阵
    Eigen::MatrixXd Q_;

    // 测量噪声协方差矩阵
    Eigen::MatrixXd R_;
};

#endif // KALMAN_FILTER_H