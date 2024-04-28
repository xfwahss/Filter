#include "../include/EnsembleKalmanFilter.h"
#include <iostream>

class Test {
  private:
    Eigen::VectorXd status;

  public:
    Eigen::VectorXd get_status() { return status; }
    void update(Eigen::VectorXd &status) {
        this->status = status;
    }
    void predict(const double &dt) {
        for(int i=0; i<status.size(); ++i){
            status(i) += dt;
        }
        // status 不变
    }
};

int main() {

    FilterIO filterio("../data/sin.txt", "../data/filter_sin.txt");
    EnsembleKalmanFilter<Test> enkal(1000);
    Eigen::VectorXd X(1);
    X << 0;
    Eigen::MatrixXd P(1, 1);
    P(0, 0)=100;
    Eigen::MatrixXd H(1, 1);
    H(0, 0) = 1;
    Eigen::VectorXd Q(1);
    Q << 1;
    Eigen::MatrixXd R(1, 1);
    R(0, 0) = 0.25;
    enkal.init(X, P, H, Q);
    enkal.batch_assimilation(filterio, 0.05, R);
}