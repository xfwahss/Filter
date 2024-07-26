#ifndef SEDIMENT_H
#define SEDIMENT_H
#include <Eigen/Dense>

class Sediment {
    double don_flux;
    double ammonia_flux;
    double nitrate_flux;

  public:
    static const int param_nums;
    void init(const double &don_flux, const double &ammonia_flux,
              const double &nitrate_flux);
    void update(const Eigen::VectorXd &s);
    double don_release();
    double ammonia_release();
    double nitrate_release();
    Eigen::VectorXd sediment_flux();
};

#endif