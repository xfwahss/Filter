#include "../include/River.h"

River::River(const std::string& filename): file(filename){
    flow = 0;
    c_no = 0;
    c_na = 0;
    c_nn = 0;
};
River::~River(){};

bool River::flush() {
    // 读取一列数据
    bool status = file.readline();
    if (status){
        flow = file.data.flow;
        c_no = file.data.organic;
        c_na = file.data.ammonia;
        c_nn = file.data.nitrate;
        load_organic = flow * c_no;
        load_ammonia = flow * c_na;
        load_nitrate = flow * c_nn;
    }
    return status;
}

double River::get_flow() { return flow; }

double River::get_load_organic() { return load_organic; }

double River::get_load_ammonia() { return load_ammonia; }

double River::get_load_nitrate() { return load_nitrate; }
