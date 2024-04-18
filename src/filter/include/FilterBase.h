#ifndef FILTER_BASE_H
#define FILTER_BASE_H
#include <Eigen/Dense>

class FilterBase {
  protected:
    Eigen::VectorXd X;
    Eigen::MatrixXd P;
  public:
    FilterBase();
    ~FilterBase();
};
#endif