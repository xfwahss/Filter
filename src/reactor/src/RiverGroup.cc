#include <RiverGroup.h>
RiverGroup::RiverGroup() : river_nums(0) {}
RiverGroup::~RiverGroup() {
    std::map<int, River *>::iterator iter = rivers.begin();
    while (iter != rivers.end()) {
        delete iter->second;
        iter->second = nullptr;
        iter++;
    }
}

void RiverGroup::add_river() {
    rivers[river_nums] = new River();
    ++river_nums;
}

double RiverGroup::get_river_nums() { return river_nums; }

void RiverGroup::update(const Eigen::VectorXd &s) {
    if (s.size() == river_nums * River::status_nums) {
        std::map<int, River *>::const_iterator iter = rivers.begin();
        int index                                   = 0;
        while (iter != rivers.end()) {
            Eigen::VectorXd ss =
                s.segment(index * River::status_nums, River::status_nums);
            iter->second->update(ss);
            ++iter;
            ++index;
        }
    } else {
        throw std::length_error(
            "The length of vectorxd input does not match the rivers' need...");
    }
};

Eigen::VectorXd RiverGroup::flow_status() {
    Eigen::VectorXd flow_status = Eigen::VectorXd::Zero(River::status_nums);
    std::map<int, River *>::const_iterator iter = rivers.begin();
    while (iter != rivers.end()) {
        flow_status += iter->second->flow_status();
        ++iter;
    }
    return flow_status;
};