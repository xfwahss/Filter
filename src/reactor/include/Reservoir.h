#ifndef RESERVOIR_H
#define RESERVOIR_H
#include "Denitrification.h"
#include "Nitrification.h"
#include "River.h"
#include <vector>

struct hidden_var{
    // 水库状态变量
    double wl;
    double c_no;
    double c_na;
    double c_nn;
    // 硝氮状态变量
};

struct reservoir_status {
    double wl;   // 水库水位
    double c_no; // 水库有机氮浓度
    double c_na; // 水库氨氮浓度
    double c_nn; // 水库硝态氮浓度
    reservoir_status() = default;
    reservoir_status(const double &wl, const double &c_no, const double &c_na,
                     const double &c_nn)
        : wl(wl), c_no(c_no), c_na(c_na), c_nn(c_nn) {}
};

class Reservoir :{
  private:
    Nitrification nitrifi_process;
    Denitrification denitri_process;
    std::vector<River*> rivers_in;
    std::vector<River*> rivers_out;
    double wl;           // 水库水位
    double get_volumn(); // 根据水库水位计算水量
  public:
    Reservoir();
    ~Reservoir();
    void add_river_in(River &river);
    void add_river_out(River &river);
    void init();
    void update_status(const reservoir_status &updated_status);
    reservoir_status current_status();
};

#endif