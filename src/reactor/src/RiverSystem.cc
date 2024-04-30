#include "../include/RiverSystem.h"

RiverSystem::RiverSystem() {}
RiverSystem::~RiverSystem() {}

void RiverSystem::add_river(River &river){
    rivers.push_back(&river);
}

Eigen::VectorXd RiverSystem::get_status(){};
void RiverSystem::update(Eigen::VectorXd status){};
void predict(const double& dt);