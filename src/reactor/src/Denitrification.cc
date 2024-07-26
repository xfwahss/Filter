#include <Denitrification.h>
const int Denitrification::param_nums = 7;
Denitrification::Denitrification() {}
Denitrification::~Denitrification() {}

void Denitrification::init(const double &r_deni0, const double &k_deni_20,
                           const double &Tc, const double &theta,
                           const double &c_oxc, const double &c_oxo,
                           const double &beta) {
    this->r_deni0   = r_deni0;
    this->k_deni_20 = k_deni_20;
    this->Tc        = Tc;
    this->theta     = theta;
    this->c_oxc     = c_oxc;
    this->c_oxo     = c_oxo;
    this->beta      = beta;
}
void Denitrification::update(const Eigen::VectorXd &s) {
    if (s.size() == param_nums) {
        r_deni0   = s(0);
        k_deni_20 = s(1);
        Tc        = s(2);
        theta     = s(3);
        c_oxc     = s(4);
        c_oxo     = s(5);
        beta      = s(6);
    } else {
        throw std::length_error(
            "The length of vector input does not match the param_nums");
    }
}

double Denitrification::fox(const double &c_ox) {
    if (c_ox <= c_oxo) {
        return 1.0;
    } else if (c_ox < c_oxc) {
        double numerator = c_oxc - c_ox;
        double denominator =
            (c_oxc - c_oxo) + (std::pow(M_E, beta) - M_E) * (c_ox - c_oxo);
        return numerator / denominator;
    } else {
        return 0.0;
    }
}

double Denitrification::ft(const double &T) {
    if (T < Tc) {
        return 0;
    } else {
        return std::pow(theta, T - 20);
    }
}

double Denitrification::rate0() { return r_deni0; }

double Denitrification::k_deni(const double &c_ox, const double &T) {
    return k_deni_20 * fox(c_ox) * ft(T);
}