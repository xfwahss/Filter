#include "../include/Reservoir.h"

Reservoir::Reservoir(const std::string &filename):file(filename, "w") {}
Reservoir::~Reservoir() {}

void Reservoir::init(const double &wl, const double &c_no, const double &c_na,
                     const double &c_nn, Nitrification &nitrifi_process,
                     Denitrification &denitri_process) {
    status.wl             = wl;
    status.c_no           = c_no;
    status.c_na           = c_na;
    status.c_nn           = c_nn;
    volumn                = wl_to_volumn(wl);
    this->nitrifi_process = nitrifi_process;
    this->denitri_process = denitri_process;
    input_output_var temp(0, 0, 0, 0, 0, 0, 0, 0);
    current_rivervars = temp;
}

void Reservoir::add_river_in(River *river) { rivers_in.push_back(river); }

void Reservoir::add_river_out(River *river) { rivers_out.push_back(river); }

double Reservoir::wl_to_volumn(const double &wl) {
    // return -1.98*1e-6*std::pow(wl, 5) + 1.43 * 1e-3 * std::pow(wl, 4) - 0.415 * std::pow(wl, 3) + 60.1 * std::pow(wl, 2) - 4350 * wl + 1.26 * 1e5;
    return 0.0228 * wl * wl - 5.3301 * wl + 313.6498;
}
// 牛顿迭代法求解1元3次方程
double Reservoir::volumn_to_wl(const double &volumn) {
    double x0 = 300, x1 = 500, epsion = 1e-8;
    do {
        double temp = x1 - (wl_to_volumn(x1) - volumn) /
                               ((wl_to_volumn(x1) - volumn) -
                                (wl_to_volumn(x0) - volumn)) *
                               (x1 - x0);
        x0 = x1;
        x1 = temp;
    } while (std::fabs(wl_to_volumn(x1) - volumn) > epsion);
    return x1;
}

input_output_var Reservoir::get_next_rivervars() {
    input_output_var next(0, 0, 0, 0, 0, 0, 0, 0);
    for (int i = 0; i < rivers_in.size(); i++) {
        // 河流读取下一时间步信息
        rivers_in[i]->flush();
        next.flow_in += rivers_in[i]->get_flow();
        next.load_organic_in += rivers_in[i]->get_load_organic();
        next.load_ammonia_in += rivers_in[i]->get_load_ammonia();
        next.load_nitrate_in += rivers_in[i]->get_load_nitrate();
    };
    for (int j = 0; j < rivers_out.size(); j++) {
        // 河流读取下一步
        rivers_out[j]->flush();
        next.flow_out += rivers_out[j]->get_flow();
        next.load_organic_out += rivers_out[j]->get_load_organic();
        next.load_ammonia_out += rivers_out[j]->get_load_ammonia();
        next.load_nitrate_out += rivers_out[j]->get_load_nitrate();
    };
    current_rivervars = next;
    return next;
}

reservoir_status Reservoir::current_status() { return status; }

void Reservoir::update_status(const reservoir_status &updated_status) {
    status = updated_status;
}

hidden_var Reservoir::current_hidden() {
    reservoir_status res        = this->current_status();
    nitrify_status nitr         = nitrifi_process.current_status();
    denitrification_status deni = denitri_process.current_status();
    hidden_var value(res, nitr, deni);
    return value;
}

void Reservoir::update_hidden(const hidden_var &update_hidden) {
    this->update_status(update_hidden.res_status);
    nitrifi_process.update_status(update_hidden.nitr_status);
    denitri_process.update_status(update_hidden.deni_status);
}

double Reservoir::predict_volumn(const double &dt) {
    double next_volumn =
        volumn + dt * (current_rivervars.flow_in - current_rivervars.flow_out) * 1e-8;
    return next_volumn;
}

double Reservoir::predict_organic(const double &dt) { return 0; }
double Reservoir::predict_ammonia(const double &dt) { return 0; }
double Reservoir::predict_nitrate(const double &dt) { return 0; }

void Reservoir::predict(const double &dt) {
    double seconds     = dt * 24 * 3600;
    double next_volumn = predict_volumn(seconds);
    double next_wl     = volumn_to_wl(next_volumn);
    double next_c_no   = predict_organic(seconds);
    double next_c_na   = predict_ammonia(seconds);
    double next_c_nn   = predict_nitrate(seconds);

    // 更新
    reservoir_status res(next_wl, next_c_no, next_c_na, next_c_nn);
    volumn = next_volumn;
    update_status(res);
}

void Reservoir::write(){
    file.writeline(status);
}