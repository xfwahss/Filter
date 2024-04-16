#ifndef RESERVOIR_H
#define RESERVOIR_H
#include "Denitrification.h"
#include "Nitrification.h"
#include "River.h"
#include <vector>

struct input_output_var {
    double flow_in;
    double flow_out;
    double load_organic_in;
    double load_ammonia_in;
    double load_nitrate_in;
    double load_organic_out;
    double load_ammonia_out;
    double load_nitrate_out;
    input_output_var() = default;
    input_output_var(const double &flow_in, const double &flow_out,
                     const double &load_organic_in,
                     const double &load_ammonia_in,
                     const double &load_nitrate_in,
                     const double &load_organic_out,
                     const double &load_ammonia_out,
                     const double &load_nitrate_out)
        : flow_in(flow_in), flow_out(flow_out),
          load_organic_in(load_organic_in), load_ammonia_in(load_ammonia_in),
          load_nitrate_in(load_nitrate_in), load_organic_out(load_organic_out),
          load_ammonia_out(load_ammonia_out),
          load_nitrate_out(load_nitrate_out) {}
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

struct hidden_var {
    reservoir_status res_status;
    nitrify_status nitr_status;
    denitrification_status deni_status;
    hidden_var() = default;
    hidden_var(const reservoir_status &res_status,
               const nitrify_status &nitr_status,
               const denitrification_status &deni_status)
        : res_status(res_status), nitr_status(nitr_status),
          deni_status(deni_status) {}
};

class Reservoir {
  private:
    Nitrification nitrifi_process;
    Denitrification denitri_process;
    std::vector<River*> rivers_in;
    std::vector<River*> rivers_out;
    reservoir_status status;
    input_output_var current_rivervars;
    // 存储当前计算的水量，避免反复求解计算的数据损失
    double volumn;
    // 根据水库水位计算水量,TODO 可能函数需要修改
    double wl_to_volumn();

    // 由水量计算出水位
    double volumn_to_wl(const double &volumn);
    /* 预测水位, 返回水量信息
    @param dt 以天为单位
    */
    double predict_volumn(const double &dt);
    /* 预测有机氮浓度
    @param dt 以天为单位
    */
    double predict_organic(const double &dt);
    /* 预测氨氮浓度
    @param dt 以天为单位
    */
    double predict_ammonia(const double &dt);
    /* 预测硝态氮浓度
    @param dt 以天为单位
    */
    double predict_nitrate(const double &dt);

  public:
    Reservoir();
    ~Reservoir();
    /* 水库状态初始化
       @param wl 水库水位
       @param c_no 水库有机氮浓度
       @param c_na 水库氨氮浓度
       @param c_nn 水库硝态氮浓度
       @param nitrifi_process 硝化系统
       @param denitri_process 反硝化系统
    */
    void init(const double &wl, const double &c_no, const double &c_na,
              const double &c_nn, Nitrification &nitrifi_process,
              Denitrification &denitri_process);
    void add_river_in(River *river);
    void add_river_out(River *river);
    // 获取下一时刻河流状态信息并更新current_rivervars
    // 这里并没有限制时间步，考虑设置时间步防止空读
    input_output_var get_next_rivervars();
    void update_status(const reservoir_status &updated_status);
    reservoir_status current_status();
    // 所有系统的状态
    hidden_var current_hidden();
    // 更新所有系统的状态
    void update_hidden(const hidden_var &updated_hidden);
    // 根据当前信息估计下一步水库状态,同时更新当前信息;
    // 也能够保证通过current_hidden获取所有的变量
    // 这步更新保证没有同化时模型能够继续跑下去
    void predict(const double &dt);
};

#endif