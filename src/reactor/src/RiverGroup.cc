#include <RiverGroup.h>

RiverGroup::RiverGroup() {}
RiverGroup::~RiverGroup() {
    std::map<int, River *>::iterator iter = rivers.begin();
    while (iter != rivers.end()) {
        delete iter->second;
        iter-> second = nullptr;
    }
}

void RiverGroup::add_river(const int& id) {
    rivers[id] = new River();
    rivers[id] ->init(0, 0, 0, 0);
}

Eigen::VectorXd RiverGroup::get_status() { return status; };

void RiverGroup::update(Eigen::VectorXd rivers_status) {
    std::map<int, River *>::const_iterator iter = rivers.begin();
    int index                                           = 0;
    while (iter != rivers.end()) {
        Eigen::VectorXd single_s = rivers_status.segment(index * river_status::size, river_status::size);
        iter->second->update(single_s);
        ++iter;
        ++index;
    }
    Eigen::VectorXd status = sum();
    this->status           = status;
};


Eigen::VectorXd RiverGroup::sum() {
    Eigen::VectorXd total_rivers = Eigen::VectorXd::Zero(4);
    std::map<int, River *>::const_iterator iter = rivers.begin();
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