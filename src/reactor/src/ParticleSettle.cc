#include <ParticleSettle.h>

const int ParticleSettle::param_nums = 2;

void ParticleSettle::init(const double &s_nrp, const double &s_nlp) {
    this->s_nrp = s_nrp;
    this->s_nlp = s_nlp;
}

void ParticleSettle::update(const Eigen::VectorXd &s) { init(s(0), s(1)); }

Eigen::VectorXd ParticleSettle::get_settle_params() {
    Eigen::VectorXd s(param_nums);
    s(0) = s_nrp;
    s(1) = s_nlp;
    return s;
}