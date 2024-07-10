#include <Reservoir.h>

Reservoir::Reservoir(){};
Reservoir::~Reservoir(){};

void Reservoir::init(const double &wl, const double &c_no, const double &c_na, const double &c_nn, const double &T,
                     const double &c_do) {
    res_status status(wl, c_no, c_na, c_nn, T, c_do);
    this->status = status;
    this->volumn = wl_to_volumn(wl);
}

void Reservoir::update(res_status &status) {
    this->status = status;
    this->volumn = wl_to_volumn(status.wl);
}

void Reservoir::update(const Eigen::VectorXd &status) {
    res_status s;
    s = status;
    update(s);
}

res_status Reservoir::get_status() { return status; }

double Reservoir::wl_to_volumn(const double &wl) { return 0.022846 * wl * wl - 5.341818 * wl + 314.495276; }

double Reservoir::volumn_to_wl(const double &volumn) {
    double a = 0.022846;
    double b = -5.341818;
    double c = 314.495276 - volumn;
    double x = (-b + std::sqrt(b * b - 4 * a * c)) / a / 2;
    return x;
}

double Reservoir::predict_volumn(const double &seconds, const double &flow_in, const double &flow_out) {
    double volumn = this->volumn + (flow_in - flow_out) * seconds * 1e-8;
    return volumn;
}

double Reservoir::predict_organic(const double &seconds, const double &load_in, const double &load_out,
                                  const double &ro, const double &next_volumn) {
    double c_v = status.c_no * volumn + (load_in - load_out) * seconds * 1e-8 - ro * volumn * seconds;
    double co  = c_v / next_volumn;
    return co;
}

double Reservoir::predict_ammonia(const double &seconds, const double &load_in, const double &load_out,
                                  const double &ro, const double &ra, const double &next_volumn) {
    double c_v =
        status.c_na * volumn + (load_in - load_out) * seconds * 1e-8 - ra * volumn * seconds + ro * volumn * seconds;
    double ca = c_v / next_volumn;
    return ca;
}

double Reservoir::predict_nitrate(const double &seconds, const double &load_in, const double &load_out,
                                  const double &ra, const double &rn, const double &next_volumn) {
    double c_v =
        status.c_nn * volumn + (load_in - load_out) * seconds * 1e-8 - rn * volumn * seconds + ra * volumn * seconds;
    double cn = c_v / next_volumn;
    return cn;
}

void Reservoir::predict(const double &dt, const double &flow_in, const double &flow_out, const double &load_org_in,
                        const double &load_org_out, const double &load_amm_in, const double &load_amm_out,
                        const double &load_nit_in, const double &load_nit_out, const double &ro, const double &ra,
                        const double &rn) {
    double seconds     = dt * 86400;
    double next_volumn = predict_volumn(seconds, flow_in, flow_out);
    double next_wl     = volumn_to_wl(next_volumn);
    double next_co     = predict_organic(seconds, load_org_in, load_org_out, ro, next_volumn);
    double next_ca     = predict_ammonia(seconds, load_amm_in, load_amm_out, ro, ra, next_volumn);
    double next_cn     = predict_nitrate(seconds, load_nit_in, load_nit_out, ra, rn, next_volumn);
    res_status update_status(next_wl, next_co, next_ca, next_cn, status.T, status.c_do);
    update(update_status);
}

Eigen::VectorXd Reservoir::predict(const Eigen::VectorXd &flow_in, const Eigen::VectorXd &flow_out, const double &ro,
                                    const double &ra, const double &rn, const double &dt) {
    int status_len = res_status::size;
    Eigen::VectorXd value(status_len);
    double seconds     = dt * 86400;
    double next_volumn = predict_volumn(seconds, flow_in(0), flow_out(0));
    double next_wl     = volumn_to_wl(next_volumn);
    double next_co     = predict_organic(seconds, flow_in(1), flow_out(1), ro, next_volumn);
    double next_ca     = predict_ammonia(seconds, flow_in(2), flow_out(2), ro, ra, next_volumn);
    double next_cn     = predict_nitrate(seconds, flow_in(3), flow_out(3), ra, rn, next_volumn);
    value << next_wl, next_co, next_ca, next_cn, status.T, status.c_do;
    return value;
}
