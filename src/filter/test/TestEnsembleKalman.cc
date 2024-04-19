#include "../include/EnsembleKalmanFilter.h"
#include <iostream>

class Test {
  private:
    Eigen::VectorXd status;

  public:
    Eigen::VectorXd current_status() { return status; }
    void update_status(Eigen::VectorXd &status) {
        this->status = status;
    }
    void predict(const double &dt) {
        // status 不变
    }
};

int main() {
    Eigen::VectorXd mean(3);
    Eigen::MatrixXd cov(3, 3);
    mean << 1, 2, 3;
    cov << 4, 0, 0, 
           0, 1, 0,
           0, 0, 1;

    EnsembleKalmanFilter<Test> enkal(10000, mean, cov);

    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
    enkal.predict(1);
    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
}