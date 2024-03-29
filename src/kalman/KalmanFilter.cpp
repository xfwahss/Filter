#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(const int& dim_state, const int& dim_measure):
    dim_state(dim_state),
    dim_measure(dim_measure)
    {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::init(const Eigen::VectorXd& x, const Eigen::MatrixXd& P,
                        const Eigen::MatrixXd& F, const Eigen::MatrixXd& Q,
                        const Eigen::MatrixXd& B, const Eigen::VectorXd& u,
                        const Eigen::MatrixXd& H, const Eigen::MatrixXd& R) {
    this -> x = x;
    this -> P = P;
    this -> F = F;
    this -> Q = Q;
    this -> B = B;
    this -> u = u;
    this -> H = H;
    this -> R = R;
}

void KalmanFilter::predict() {
    x = F * x + B * u;
    P = F * P * F.transpose() + Q;
}

void KalmanFilter::update(const Eigen::VectorXd& measurement) {
    Eigen::MatrixXd K = P * H.transpose() * (H * P * H.transpose() + R).inverse();
    x = x + K * (measurement - H * x);
    P = (Eigen::MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P;
}

Eigen::VectorXd KalmanFilter::get_state() const {
    return x;
}

Eigen::MatrixXd KalmanFilter::get_covariance() const{
    return P;
}

void KalmanFilter::batch_filter(const int &counts, const double* measurements){
    for (int i = 0; i < counts; ++i) {
        this -> predict();
        Eigen::VectorXd measurement(1);
        measurement << measurements[i];
        this -> update(measurement);
        Eigen::VectorXd estimated_state = this->get_state();
        Eigen::MatrixXd covariance = this->get_covariance();
        std::cout << "Estimated State: " << estimated_state << std::endl;
        std::cout << "Covariance: " << covariance << std::endl;
    }

}
