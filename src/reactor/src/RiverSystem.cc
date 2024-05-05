#include "../include/RiverSystem.h"

RiverSystem::RiverSystem() {}
RiverSystem::~RiverSystem() {}

void RiverSystem::add_river(std::string river_name, River &river) {
    rivers[river_name] = &river;
}

// Eigen::VectorXd RiverSystem::get_status(){};
void RiverSystem::update(Eigen::VectorXd status){};
void predict(const double &dt);
std::ostream &operator<<(std::ostream &os, const RiverSystem &river) {
    std::map<std::string, River *>::const_iterator iter = river.rivers.begin();
    while (iter != river.rivers.end()) {
        os << iter->first << std::endl;
        ++iter;
    }
    return os;
}