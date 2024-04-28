#include "../include/EnsembleKalmanFilter.h"
#include <iostream>

class Test {
  private:
    Eigen::VectorXd status;

  public:
    Eigen::VectorXd get_status() { return status; }
    void update(Eigen::VectorXd &status) { this->status = status; }
    void predict(const double &dt) {
        // status 不变
    }
};

int main() {
    Eigen::VectorXd mean(3);
    Eigen::MatrixXd cov(3, 3);

    Eigen::VectorXd Q(3);
    Q << 1, 1, 1;
    mean << 1, 2, 3;
    cov << 4, 0.4, 0.8, 0.4, 1, 0.2, 0.8, 0.2, 1;

    EnsembleKalmanFilter<Test> enkal(1000);
    enkal.init(mean, cov, Q, Q);
    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
    enkal.predict(1);
    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
    enkal.predict(1);
    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;

    Eigen::VectorXd X(1);
    X << 1;
    Eigen::MatrixXd P(1, 1);
    P(0, 0) = 2;
    Eigen::MatrixXd H(1, 1);
    H(0, 0) = 1;
    Eigen::VectorXd Q2(1);
    Q2 << 0.01;
    enkal.init(X, P, H, Q2);
    Eigen::MatrixXd measurements(1, 1);
    double arr[5] = {2.5, 2.4, 2.6, 2.8, 2.9};
    Eigen::MatrixXd R(1, 1);
    R(0, 0) = 0.25;
    // measurements << 2.5, 2.4, 2.6, 2.8, 2.9;
    for (int i = 0; i < 5; i++) {
        measurements(0, 0) = arr[i];
        enkal.step_assimilation(1, measurements, R);
        std::cout << enkal.current_status() << std::endl;
        std::cout << enkal.current_covariance() << std::endl;
    }
}