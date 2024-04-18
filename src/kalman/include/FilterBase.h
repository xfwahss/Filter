#ifndef FILTER_BASE_H
#define FILTER_BASE_H
#include <Eigen/Dense>

class FilterBase {
  protected:
    Eigen::VectorXd X;
    Eigen::MatrixXd P;

  public:
    virtual void predict() = 0;
    virtual void update(const Eigen::VectorXd &measurement) = 0;
};
#endif