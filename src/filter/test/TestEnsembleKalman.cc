#include "../include/EnsembleKalmanFilter.h"
#include <iostream>

class Test {
  private:
    Eigen::VectorXd status;
    int i = 0;

  public:
    Eigen::VectorXd current_status() { return status; }
    void update_status(Eigen::VectorXd &status) {
        this->status = status;
        std::cout << ++i << std::endl;
    }
    void predict(const double &dt) {
        // status 不变
    }
};

int main() {
    Eigen::VectorXd mean(2);
    Eigen::MatrixXd cov(2, 2);
    mean << 1, 2;
    cov << 4, 2, 2, 1;
    EnsembleKalmanFilter<Test> enkal(10, mean, cov);

    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
    enkal.predict(1);
    std::cout << enkal.current_status() << std::endl;
    std::cout << enkal.current_covariance() << std::endl;
}