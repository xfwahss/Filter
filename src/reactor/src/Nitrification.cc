#include "../include/Nitrification.h"

Nitrification::Nitrification(){};
Nitrification::~Nitrification(){};
double Nitrification::fox(const double &c_ox) {
    if (c_ox <= c_oxc) {
        return foxmin;
    } else if (c_ox < c_oxo) {
        return (1 - foxmin) * std::pow((c_ox - c_oxc) / (c_oxo - c_oxc),
                                       std::pow(10, alpha)) +
               foxmin;
    } else {
        return 1.0;
    }
};

double Nitrification::ka1(const double &T) {
    if (T < T_c) {
        return 0.0;
    } else {
        return kab1 * std::pow(theta_a, T - 20);
    }
}

void Nitrification::init(const double &ra0, const double &kab1,
                         const double &foxmin, const double &c_oxc,
                         const double &c_oxo, const double &theta_a,
                         const double &T_c, const double &alpha) {
    this->ra0     = ra0;
    this->kab1    = kab1;
    this->foxmin  = foxmin;
    this->c_oxc   = c_oxc;
    this->c_oxo   = c_oxo;
    this->theta_a = theta_a;
    this->T_c     = T_c;
    this->alpha   = alpha;
};

double Nitrification::rate(const double &c_ox, const double &T,
                           const double &c_am) {
    double fox  = this->fox(c_ox);
    double ka1  = this->ka1(T);
    double rate = ra0 + fox * ka1 * c_am;
    return rate;
}

nitrify_status Nitrification::current_status() {
    nitrify_status status(ra0, kab1, foxmin, c_oxc, c_oxo, theta_a, T_c);
    return status;
}

void Nitrification::update_status(const nitrify_status &update_status) {
    this->ra0     = update_status.ra0;
    this->kab1    = update_status.kab1;
    this->foxmin  = update_status.foxmin;
    this->c_oxc   = update_status.c_oxc;
    this->c_oxo   = update_status.c_oxo;
    this->theta_a = update_status.theta_a;
    this->T_c     = update_status.T_c;
}
