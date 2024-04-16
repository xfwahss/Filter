#ifndef RIVER_H
#define RIVER_H
#include <fstream>
#include <string>
#include "../../io/include/RiverFileIO.h"
class River {
  private:
    double flow;
    double c_no;
    double c_na;
    double c_nn;
    double load_organic;
    double load_ammonia;
    double load_nitrate;
    RiverFileIO file;

  public:
    River(const std::string& filename);
    ~River();
    // 返回true说明产生了新的数据
    bool flush();
    double get_flow();
    double get_load_organic();
    double get_load_ammonia();
    double get_load_nitrate();
};
#endif