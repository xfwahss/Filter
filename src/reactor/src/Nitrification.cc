#include <Nitrification.h>
const int Nitrification::param_nums = 8;

Nitrification::Nitrification(){};
Nitrification::~Nitrification(){};

void Nitrification::init(const double &rnit0, const double &knit_20,
                         const double &foxmin, const double &c_oxc,
                         const double &c_oxo, const double &theta,
                         const double &T_c, const double &alpha) {
    this->rnit0   = rnit0;
    this->knit_20 = knit_20;
    this->foxmin  = foxmin;
    this->c_oxc   = c_oxc;
    this->c_oxo   = c_oxo;
    this->theta   = theta;
    this->T_c     = T_c;
    this->alpha   = alpha;
};
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

double Nitrification::ft(const double &T) {
    if (T < T_c) {
        return 0.0;
    } else {
        return std::pow(theta, T - 20);
    }
}

void Nitrification::update(const Eigen::VectorXd &s) {
    if (s.size() == param_nums) {
        rnit0   = s(0);
        knit_20 = s(1);
        foxmin  = s(2);
        c_oxc   = s(3);
        c_oxo   = s(4);
        theta   = s(5);
        T_c     = s(6);
        alpha   = s(7);
    } else {
        throw std::length_error(
            "The length of vector input does not match the param_nums");
    }
}

double Nitrification::rate_0() { return rnit0; }

double Nitrification::knit(const double &c_ox, const double &T) {
    return knit_20 * fox(c_ox) * ft(T);
}
