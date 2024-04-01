#ifndef INIT_STATE_H
#define INIT_STATE_H
#include <Eigen/Dense>
#include <string>

class InitState {
  private:
    Eigen::VectorXd x;
    Eigen::MatrixXd P;
    Eigen::MatrixXd F;
    Eigen::MatrixXd B;
    Eigen::VectorXd u;
    Eigen::MatrixXd H;
    Eigen::MatrixXd Q;
    Eigen::MatrixXd R;
  public:
    void set_matrix(const std::string& name, const Eigen::MatrixXd& value);
    void set_vector(const std::string& name, const Eigen::VectorXd& value);
    Eigen::MatrixXd get_matrix(const std::string& name);
    Eigen::VectorXd get_vector(const std::string& name);
};
#endif