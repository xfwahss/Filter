#ifndef RIVER_H
#define RIVER_H
#include <fstream>
#include <string>
class River {
  private:
    int time_step;
    double flow;
    double c_no;
    double c_na;
    double c_nn;
    double load_organic;
    double load_ammonia;
    double load_nitrate;
    std::ifstream file;

  public:
    River(const int &time_step, const std::string filename);
    ~River();
    void read_one();
    double get_flow();
    double get_load_organic();
    double get_load_ammonia();
    double get_load_nitrate();
};
#endif