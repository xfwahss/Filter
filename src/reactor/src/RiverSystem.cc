#include "../include/RiverSystem.h"

RiverSystem::RiverSystem() {}
RiverSystem::~RiverSystem() {}

void RiverSystem::add_river(std::string river_name, River &river) {
    rivers[river_name] = &river;
}

Eigen::VectorXd RiverSystem::get_status() { return status; };

void RiverSystem::update(Eigen::VectorXd rivers_status) {
    std::map<std::string, River *>::const_iterator iter = rivers.begin();
    int index                                           = 0;
    while (iter != rivers.end()) {
        double flow = rivers_status(index * 4 + 0);
        double c_no = rivers_status(index * 4 + 1);
        double c_na = rivers_status(index * 4 + 2);
        double c_nn = rivers_status(index * 4 + 3);
        river_status s(flow, c_no, c_na, c_nn);
        iter->second->update(s);
        ++iter;
        ++index;
    }
    Eigen::VectorXd status = sum();
    this->status           = status;
};

std::ostream &operator<<(std::ostream &os, const RiverSystem &river) {
    std::map<std::string, River *>::const_iterator iter = river.rivers.begin();
    while (iter != river.rivers.end()) {
        os << iter->first << std::endl;
        ++iter;
    }
    return os;
}

Eigen::VectorXd RiverSystem::sum() {
    Eigen::VectorXd total_rivers = Eigen::VectorXd::Zero(4);
    std::map<std::string, River *>::const_iterator iter = rivers.begin();
    while (iter != rivers.end()) {
        river_status s = iter->second->get_status();
        total_rivers(0) += s.flow;
        total_rivers(1) += s.load_organic;
        total_rivers(2) += s.load_ammonia;
        total_rivers(3) += s.load_nitrate;
        ++iter;
    }
    return total_rivers;
};