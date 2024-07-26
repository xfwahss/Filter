#include <River.h>

const int River::status_nums = 6;
River::River(){};
River::~River(){};

void River::init(const double &flow, const double &c_rpon, const double &c_lpon,
                 const double &c_don, const double &c_na, const double &c_nn) {
    this->flow   = flow;
    this->c_rpon = c_rpon;
    this->c_lpon = c_lpon;
    this->c_don  = c_don;
    this->c_na   = c_na;
    this->c_nn   = c_nn;
}

void River::update(const Eigen::VectorXd &s) {
    if (s.size() == status_nums) {
        this->init(s(0), s(1), s(2), s(3), s(4), s(5));
    } else {
        throw std::length_error(
            "The length of the vector input does not match the status_nums...");
    }
}

double River::flow_rate() { return flow; }
double River::load_rpon() { return flow * c_rpon; }
double River::load_lpon() { return flow * c_lpon; }
double River::load_don() { return flow * c_don; }
double River::load_na() { return flow * c_na; }
double River::load_nn() { return flow * c_nn; }

Eigen::VectorXd River::flow_status() {
    Eigen::VectorXd s = Eigen::VectorXd::Zero(status_nums);
    s(0)              = flow_rate();
    s(1)              = load_rpon();
    s(2)              = load_lpon();
    s(3)              = load_don();
    s(4)              = load_na();
    s(5)              = load_nn();
    return s;
}