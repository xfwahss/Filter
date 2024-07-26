#include <Sediment.h>

const int Sediment::param_nums = 3;

void Sediment::init(const double &don_flux, const double &ammonia_flux,
                    const double &nitrate_flux) {
    this->don_flux     = don_flux;
    this->ammonia_flux = ammonia_flux;
    this->nitrate_flux = nitrate_flux;
}
void Sediment::update(const Eigen::VectorXd &s) {
    if (s.size() == param_nums) {
        init(s(0), s(1), s(2));
    } else {
        throw std::length_error("The size of vector input does not match "
                                "param_nums of Sediment...");
    }
}
double Sediment::don_release() { return don_flux; }
double Sediment::ammonia_release() { return ammonia_flux; }
double Sediment::nitrate_release() { return nitrate_flux; }

Eigen::VectorXd Sediment::sediment_flux() {
    Eigen::VectorXd s(param_nums);
    s(0) = don_flux;
    s(1) = ammonia_flux;
    s(2) = nitrate_flux;
    return s;
}
