#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include <Eigen/Dense>

class EnsembleKalmanFilter {
  private:
    int ensemble_size;
    // define a function pointer, receive a vector as parameter and return a
    // vector
    Eigen::VectorXd (*predict_func)(Eigen::VectorXd);

  public:
    EnsembleKalmanFilter(const int &ensemble_size,
                         Eigen::VectorXd (*const predict_func)(Eigen::VectorXd));
    ~EnsembleKalmanFilter();
    void predict();
    void update();
};
#endif