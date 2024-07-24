#ifndef AMMONIFICATION_H
#define AMMONIFICATION_H
#include "DataStructures.h"
class Ammonification {
  private:
    AmmonificationStatus status;

  public:
    Ammonification()  = default;
    ~Ammonification() = default;
    void init(const double &ro0, const double &ko1);
    void update(const AmmonificationStatus &updated_status);
    void update(const Eigen::VectorXd &status);
    double rate(const double &c_no);
};
#endif