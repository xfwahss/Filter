#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include"../../io/include/FilterIO.h"
#include<Eigen/Dense>
#include<iostream>
#include<string>

class KalmanFilter {
public:
    KalmanFilter(const int& dim_state, const int& dim_measure);
    ~KalmanFilter();

    void init(const Eigen::VectorXd& x, const Eigen::MatrixXd& P, 
              const Eigen::MatrixXd& F, const Eigen::MatrixXd& Q,
              const Eigen::MatrixXd& B, const Eigen::VectorXd& u,
              const Eigen::MatrixXd& H, const Eigen::MatrixXd& R);
    void predict();
    void update(const Eigen::VectorXd& measurement);
    Eigen::VectorXd get_state() const;
    Eigen::MatrixXd get_covariance() const;
    void batch_filter(const int &counts, const double* measurements);
    void batch_filter(FilterIO& measurements);
    void write(FilterIO& results, const std::string& variable); // write results

private:
    const int dim_state;
    const int dim_measure;
    Eigen::VectorXd x;
    Eigen::MatrixXd P;
    Eigen::MatrixXd F;
    Eigen::MatrixXd B;
    Eigen::VectorXd u;
    Eigen::MatrixXd H;
    Eigen::MatrixXd Q;
    Eigen::MatrixXd R;
};
#endif