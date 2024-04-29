#include "../include/Denitrification.h"

Denitrification::Denitrification() {}
Denitrification::~Denitrification() {}

void Denitrification::init(const double &rn0, const double &knb1,
                           const double &Tnc, const double &theta_n,
                           const double &c_noxc, const double &c_noxo,
                           const double &beta) {
    deni_status value(rn0, knb1, Tnc, theta_n, c_noxc, c_noxo);
    coeffecients = value;
    this->beta   = beta;
}

deni_status Denitrification::get_status() { return coeffecients; }

void Denitrification::update(const deni_status &updated_status) {
    coeffecients = updated_status;
}

double Denitrification::fnox(const double &c_ox) {
    if (c_ox <= coeffecients.c_noxo) {
        return 1.0;
    } else if (c_ox < coeffecients.c_noxc) {
        double numerator = coeffecients.c_noxc - c_ox;
        double denominator =
            (coeffecients.c_noxc - coeffecients.c_noxo) +
            (std::pow(M_E, beta) - M_E) * (c_ox - coeffecients.c_noxo);
        return numerator / denominator;
    } else {
        return 0.0;
    }
}

double Denitrification::kn1(const double &T) {
    if (T < coeffecients.Tnc) {
        return 0;
    } else {
        return coeffecients.knb1 * std::pow(coeffecients.theta_n, T - 20);
    }
}

double Denitrification::rate(const double &c_ox, const double &T,
                             const double &c_ni) {
    double fnox = this->fnox(c_ox);
    double kn1  = this->kn1(T);
    return coeffecients.rn0 + fnox * kn1 * c_ni;
}