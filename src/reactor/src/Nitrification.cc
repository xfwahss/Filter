#include <Nitrification.h>

Nitrification::Nitrification(){};
Nitrification::~Nitrification(){};
double Nitrification::fox(const double &c_ox) {
    if (c_ox <= status.c_oxc) {
        return status.foxmin;
    } else if (c_ox < status.c_oxo) {
        return (1 - status.foxmin) * std::pow((c_ox - status.c_oxc) /
                                                  (status.c_oxo - status.c_oxc),
                                              std::pow(10, alpha)) +
               status.foxmin;
    } else {
        return 1.0;
    }
};

double Nitrification::ka1(const double &T) {
    if (T < status.T_c) {
        return 0.0;
    } else {
        return status.kab1 * std::pow(status.theta_a, T - 20);
    }
}

void Nitrification::init(const double &ra0, const double &kab1,
                         const double &foxmin, const double &c_oxc,
                         const double &c_oxo, const double &theta_a,
                         const double &T_c, const double &alpha) {
    status.ra0     = ra0;
    status.kab1    = kab1;
    status.foxmin  = foxmin;
    status.c_oxc   = c_oxc;
    status.c_oxo   = c_oxo;
    status.theta_a = theta_a;
    status.T_c     = T_c;
    this->alpha    = alpha;
};

double Nitrification::rate(const double &c_ox, const double &T,
                           const double &c_am) {
    double fox  = this->fox(c_ox);
    double ka1  = this->ka1(T);
    double rate = status.ra0 + fox * ka1 * c_am;
    return rate;
}

nitri_status Nitrification::get_status() { return status; }

void Nitrification::update(const nitri_status &update_status) {
    status.ra0     = update_status.ra0;
    status.kab1    = update_status.kab1;
    status.foxmin  = update_status.foxmin;
    status.c_oxc   = update_status.c_oxc;
    status.c_oxo   = update_status.c_oxo;
    status.theta_a = update_status.theta_a;
    status.T_c     = update_status.T_c;
}

void Nitrification::update(const Eigen::VectorXd& status){
    nitri_status s;
    s = status;
    update(s);
}
