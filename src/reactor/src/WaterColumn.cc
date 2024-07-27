#include <WaterColumn.h>
#include <ode.h>

const int WaterColumn::status_nums       = 8;
const int WaterColumn::flow_bnd_nums     = 12;
const int WaterColumn::sediment_bnd_nums = 3;
const int WaterColumn::settle_param_nums = 2;
WaterColumn::WaterColumn() {
    wl_to_volumn = [](const double &wl) {
        return 0.022846 * wl * wl - 5.341818 * wl + 314.495276;
    };
    // TODO 需要修改时候密云水库
    wl_to_depth  = [](const double &wl) { return wl; };
    volumn_to_wl = [](const double &volumn) {
        double a = 0.022846;
        double b = -5.341818;
        double c = 314.495276 - volumn;
        double x = (-b + std::sqrt(b * b - 4 * a * c)) / a / 2;
        return x;
    };
}

WaterColumn::WaterColumn(
    const std::function<double(const double &)> &wl_to_volumn,
    const std::function<double(const double &)> &wl_to_depth,
    const std::function<double(const double &)> &volumn_to_wl) {
    this->wl_to_volumn = wl_to_volumn;
    this->wl_to_depth  = wl_to_depth;
    this->volumn_to_wl = volumn_to_wl;
}
WaterColumn::~WaterColumn(){};

void WaterColumn::init(const double &water_level, const double &c_rpon,
                       const double &c_lpon, const double &c_don,
                       const double &c_ammonia, const double &c_nitrate,
                       const double &temperature, const double &c_oxygen) {
    this->water_level = water_level;
    this->c_rpon      = c_rpon;
    this->c_lpon      = c_lpon;
    this->c_don       = c_don;
    this->c_ammonia   = c_ammonia;
    this->c_nitrate   = c_nitrate;
    this->temperature = temperature;
    this->c_oxygen    = c_oxygen;
    this->volumn      = wl_to_volumn(water_level);
    this->depth       = wl_to_depth(water_level);
}

void WaterColumn::update_wl_volumn_function(
    std::function<double(const double &wl)> wl_to_volumn,
    std::function<double(const double &wl)> wl_to_depth,
    std::function<double(const double &volumn)> volumn_to_wl) {
    this->wl_to_volumn = wl_to_volumn;
    this->wl_to_depth  = wl_to_depth;
    this->volumn_to_wl = volumn_to_wl;
}

double WaterColumn::step_volumn(const double &seconds, const double &flow_in,
                                const double &flow_out) {
    // 与volumn和t无关，因此不使用runge-kutta法
    double predict_volumn = volumn + (flow_in - flow_out) * seconds * 1e-8;
    return predict_volumn;
}

double WaterColumn::step_lpon(const double &seconds, const double &load_in,
                              const double &load_out, const double &true_k_lpon,
                              const double &s_nlp) {

    double wnlp = load_in - load_out;
    std::function<double(const double &t, const double &c)> f_t_c =
        [&wnlp, &true_k_lpon, &s_nlp, this](const double &t,
                                            const double &c) -> double {
        return -true_k_lpon * c / 86400 + wnlp / this->volumn * 1e-8 -
               s_nlp / this->depth / 86400 * c;
    };
    // 这里龙格库塔计算的与时间无关，因此传0过去无所谓
    return ODE::rungekutta4(f_t_c, 0, c_lpon, seconds);
}

double WaterColumn::step_rpon(const double &seconds, const double &load_in,
                              const double &load_out, const double &true_k_rpon,
                              const double &s_nrp) {
    double wnrp = load_in - load_out;
    std::function<double(const double &t, const double &c)> f_t_c =
        [&wnrp, &true_k_rpon, &s_nrp, this](const double &t,
                                            const double &c) -> double {
        return -true_k_rpon * c / 86400 + wnrp / this->volumn * 1e-8 -
               s_nrp / this->depth / 86400 * c;
    };
    return ODE::rungekutta4(f_t_c, 0, c_rpon, seconds);
}

double WaterColumn::step_don(const double &seconds, const double &load_in,
                             const double &load_out, const double &true_k_rpon,
                             const double &true_k_lpon,
                             const double &true_k_don, const double &bndo) {
    double wndo = load_in - load_out;
    std::function<double(const double &t, const double &c)> f_t_c =
        [&wndo, &true_k_rpon, &true_k_lpon, &true_k_don, &bndo,
         this](const double &t, const double &c) -> double {
        return true_k_rpon * this->c_rpon / 86400 +
               true_k_lpon * this->c_lpon / 86400 - true_k_don * c / 86400 +
               wndo / this->volumn * 1e-8 + bndo / this->depth / 86400;
    };
    return ODE::rungekutta4(f_t_c, 0, c_don, seconds);
}

double WaterColumn::step_ammonia(const double &seconds, const double &load_in,
                                 const double &load_out,
                                 const double &true_k_don, const double &r_nit0,
                                 const double &true_k_nit,
                                 const double &bf_ammonia) {
    double wna = load_in - load_out;
    std::function<double(const double &t, const double &c)> f_t_c =
        [&wna, &true_k_don, &r_nit0, &true_k_nit, &bf_ammonia,
         this](const double &t, const double &c) -> double {
        return true_k_don * this->c_don / 86400 - r_nit0 / 86400 -
               true_k_nit * c / 86400 + wna / this->volumn * 1e-8 +
               bf_ammonia / this->depth / 86400;
    };
    return ODE::rungekutta4(f_t_c, 0, c_ammonia, seconds);
}

double WaterColumn::step_nitrate(const double &seconds, const double &load_in,
                                 const double &load_out, const double &rnit0,
                                 const double &true_k_nit,
                                 const double &rdenit0,
                                 const double &true_k_deni,
                                 const double &bf_nitrate) {
    double wnn = load_in - load_out;
    std::function<double(const double &t, const double &c)> f_t_c =
        [&wnn, &rnit0, &true_k_nit, &rdenit0, &true_k_deni, &bf_nitrate,
         this](const double &t, const double &c) -> double {
        return rnit0 / 86400 + true_k_nit * this->c_ammonia / 86400 -
               rdenit0 / 86400 - true_k_deni * c / 86400 +
               wnn / this->volumn * 1e-8 + bf_nitrate / this->depth / 86400;
    };

    return ODE::rungekutta4(f_t_c, 0, c_nitrate, seconds);
}

void WaterColumn::update(const Eigen::VectorXd &s) {
    if (s.size() == status_nums) {
        water_level = s(0);
        c_rpon      = s(1);
        c_lpon      = s(2);
        c_don       = s(3);
        c_ammonia   = s(4);
        c_nitrate   = s(5);
        temperature = s(6);
        c_oxygen    = s(7);
        volumn      = wl_to_volumn(water_level);
        depth       = wl_to_depth(water_level);
    } else {
        throw std::length_error("The size of vector input does not match the "
                                "param_nums of WaterColumn...");
    }
}

Eigen::VectorXd WaterColumn::step(const double &dt,
                                  const Eigen::VectorXd &flow_bnd,
                                  const Eigen::VectorXd &sediment_bnd,
                                  const Eigen::VectorXd &settle_params,
                                  Ammonification &sys_ammonification,
                                  Nitrification &sys_nitrification,
                                  Denitrification &sys_denitrification) {
    /* next_status的格式，当前并未实现温度和溶解氧的估计
     * [water_level, c_rpon, c_lpon, c_don, c_ammonia, c_nitrate, temperature,
     * c_oxygen]
     */
    Eigen::VectorXd next_status = Eigen::VectorXd::Zero(status_nums);
    if (flow_bnd.size() != flow_bnd_nums) {
        throw std::length_error("The size of flow_bnd does not match the "
                                "flow_bnd_nums of WaterColumn...");
    } else if (sediment_bnd.size() != sediment_bnd_nums) {
        throw std::length_error("The size of sediment_bnd does not match the "
                                "sediment_bnd_nums of WaterColumn...");
    } else if (settle_params.size() != settle_param_nums) {
        throw std::length_error("The size of settle_params does not match the "
                                "settle_param_nums of WaterColumn...");
    } else {
        int offset         = flow_bnd_nums / 2;
        double next_volumn = step_volumn(dt, flow_bnd(0), flow_bnd(offset + 0));
        next_status(0)     = volumn_to_wl(next_volumn);
        next_status(1) =
            step_rpon(dt, flow_bnd(1), flow_bnd(1 + offset),
                      sys_ammonification.k_rpon_hydrolysis(temperature),
                      settle_params(0));
        next_status(2) =
            step_lpon(dt, flow_bnd(2), flow_bnd(offset + 2),
                      sys_ammonification.k_lpon_hydrolysis(temperature),
                      settle_params(1));
        next_status(3) =
            step_don(dt, flow_bnd(3), flow_bnd(offset + 3),
                     sys_ammonification.k_rpon_hydrolysis(temperature),
                     sys_ammonification.k_lpon_hydrolysis(temperature),
                     sys_ammonification.k_don_mineralization(temperature),
                     sediment_bnd(0));
        next_status(4) = step_ammonia(
            dt, flow_bnd(4), flow_bnd(offset + 4),
            sys_ammonification.k_don_mineralization(temperature),
            sys_nitrification.rate_0(),
            sys_nitrification.knit(c_oxygen, temperature), sediment_bnd(1));
        next_status(5) = step_nitrate(
            dt, flow_bnd(5), flow_bnd(offset + 5), sys_nitrification.rate_0(),
            sys_nitrification.knit(c_oxygen, temperature),
            sys_denitrification.rate0(),
            sys_denitrification.k_deni(c_oxygen, temperature), sediment_bnd(2));
        next_status(6) = temperature;
        next_status(7) = c_oxygen;
    }
    return next_status;
}

Eigen::VectorXd WaterColumn::predict(
    const Eigen::VectorXd &s0, const double &dt,
    const Eigen::VectorXd &flow_bnd, const Eigen::VectorXd &sediment_bnd,
    const Eigen::VectorXd &settle_params, Ammonification &sys_ammonification,
    Nitrification &sys_nitrification, Denitrification &sys_denitrification) {
    update(s0);
    Eigen::VectorXd next_s =
        step(dt, flow_bnd, sediment_bnd, settle_params, sys_ammonification,
             sys_nitrification, sys_denitrification);
    return next_s;
}
