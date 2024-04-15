#include "../include/River.h"

River::River(const int &time_step, const std::string filename){};
River::~River(){};

void River::read_one() {
    time_step++;
    flow         = 0;
    c_no         = 0;
    c_na         = 0;
    c_nn         = 0;
    load_organic = flow * c_no;
    load_ammonia = flow * c_na;
    load_nitrate = flow * c_nn;
}

double River::get_flow() { return flow; }

double River::get_load_organic() { return load_organic; }

double River::get_load_ammonia() { return load_ammonia; }

double River::get_load_nitrate() { return load_nitrate; }
