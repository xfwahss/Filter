#ifndef WATERCOLUMN_H
#define WATERCOLUMN_H
#include <Ammonification.h>
#include <Denitrification.h>
#include <Eigen/Dense>
#include <Nitrification.h>
#include <functional>

class WaterColumn {
  private:
    // 水位状态变量
    double water_level;
    double c_rpon, c_lpon, c_don, c_ammonia, c_nitrate;
    double temperature, c_oxygen;
    // 水量单位10^8 m3
    double volumn;
    double depth;

    std::function<double(const double &)> wl_to_volumn;
    std::function<double(const double &)> wl_to_depth;
    std::function<double(const double &)> volumn_to_wl;
    double step_volumn(const double &seconds, const double &flow_in,
                       const double &flow_out);
    /*
     * @brief 由当前时刻的c_lpon计算下一时间步c_lpon(易降解颗粒有机氮)
     * @param seconds 时间,秒
     * @param load_in 易降解颗粒有机氮入库负荷 g/s
     * @param load_out 易降解颗粒有机氮出库负荷 g/s
     * @param k_lpon 易降解颗粒有机氮水解速率常数，1/d
     * @param s_nlp  易降解颗粒有机氮沉降速率，m/d
     * @param H 水库平均水深m
     */
    double step_lpon(const double &seconds, const double &load_in,
                     const double &load_out, const double &true_k_lpon,
                     const double &s_nlp);

    /*
     * @brief 由当前时刻的c_rpon计算下一时间步c_rpon(难降解颗粒有机氮)
     * @param seconds 时间,秒
     * @param load_in 难降解颗粒有机氮入库负荷 g/s
     * @param load_out 难降解颗粒有机氮出库负荷 g/s
     * @param true_k_rpon 难降解颗粒有机氮水解速率常数，1/d
     * @param s_nrp  难降解颗粒有机氮沉降速率，m/d
     */
    double step_rpon(const double &seconds, const double &load_in,
                     const double &load_out, const double &true_k_rpon,
                     const double &s_nrp);

    /*
     * @brief 由当前时刻的c_don计算下一时间步c_don(溶解性有机氮)
     * @param seconds 时间,秒
     * @param load_in 溶解性有机氮入库负荷 g/s
     * @param load_out 溶解性有机氮出库负荷 g/s
     * @param true_k_rpon 难降解颗粒有机氮水解速率常数，1/d
     * @param true_k_lpon 易降解颗粒有机氮水解速率常数，1/d
     * @param true_k_don 溶解性有机氮矿化速率常数，1/d
     * @param bndo  沉积物-水界面溶解性有机氮交换通量，gNm-2d
     */
    double step_don(const double &seconds, const double &load_in,
                    const double &load_out, const double &true_k_rpon,
                    const double &true_k_lpon, const double &true_k_don,
                    const double &bndo);
    /*
     * @brief 由当前时刻的c_ammonia计算下一时间步的c_ammonia(氨氮浓度)
     * @param seconds 时间秒
     * @param load_in 氨氮入库负荷 g/s
     * @param load_out 氨氮出库负荷 g/s
     * @param true_k_don 溶解性有机氮矿化反应速率常数 1/d
     * @param r_nit0 0级硝化反应速率 g/m3/d
     * @param true_k_nit 1级硝化反应速率常数 1/d
     * @param bf_ammonia 沉积物水界面氨氮界面交换通量 g/m2/d
     */
    double step_ammonia(const double &seconds, const double &load_in,
                        const double &load_out, const double &true_k_don,
                        const double &r_nit0, const double &true_k_nit,
                        const double &bf_ammonia);
    /*
     * @brief 由当前时间步的c_nitrate计算下一时间步的c_nitrate(硝态氮浓度)
     * @param seconds 时间秒
     * @param load_in 硝氮入库负荷 g/s
     * @param load_out 硝氮出库负荷 g/s
     * @param rnit0 0级硝化速率 g/m3/d
     * @param true_k_nit 1级硝化速率常数 1/d
     * @param rdenit0 0级反硝化速率 g/m3/d
     * @param true_k_deni 1级反硝化速率常数 1/d
     * @param bf_nitrate 沉积物水界面硝氮交换通量 g/m2/d
     */
    double step_nitrate(const double &seconds, const double &load_in,
                        const double &load_out, const double &rnit0,
                        const double &true_k_nit, const double &rdenit0,
                        const double &true_k_deni, const double &bf_nitrate);

  public:
    // 仅统计独一无二的参数个数，例如water level和volumn就当作一个参数
    static const int status_nums;
    // 纯粹流动边界条件统计
    static const int flow_bnd_nums;
    static const int sediment_bnd_nums;
    static const int settle_param_nums;
    WaterColumn();
    WaterColumn(const std::function<double(const double &)> &wl_to_volumn,
                const std::function<double(const double &)> &wl_to_depth,
                const std::function<double(const double &)> &volumn_to_wl);
    ~WaterColumn();
    void init(const double &water_level, const double &c_rpon,
              const double &c_lpon, const double &c_don,
              const double &c_ammonia, const double &c_nitrate,
              const double &temperature, const double &c_oxygen);

    void update_wl_volumn_function(
        std::function<double(const double &wl)> wl_to_volumn,
        std::function<double(const double &wl)> wl_to_depth,
        std::function<double(const double &volumn)> volumn_to_wl);

    /* @brief 更新当前的水库状态
     * @param s: 状态向量输入格式
     * [water_level, c_rpon, c_lpon, c_don, c_ammonia, c_nitrate,
     * temperature, c_oxygen]
     */
    void update(const Eigen::VectorXd &s);

    /* @brief 由当前的水库状态向前推进dt,并返回新的水库状态
     * @param dt 推进时间步长,单位s
     * @param flow_bnd 当前时刻流动边界条件，格式如下(12个)：
     * [flow_in, load_rpon_in, load_lpon_in, load_don_in, load_amm_in,
     * load_nit_in, flow_out, load_rpon_out, load_lpon_out, load_don_out,
     * load_amm_out, load_nit_out]
     * @param sediment_bnd 当前时刻的底泥边界条件，格式如下(3个)：
     * [bndo, bna, bnn], 分别为溶解性有机氮，氨氮和硝氮的释放量
     * @param settle_params 颗粒有机氮的沉降参数，存储格式为：
     * [s_nrp, s_nlp]
     * @param Ammonification 氨化系统
     * @param Nitrification 硝化系统
     * @param Denitrification 反硝化系统
     */
    Eigen::VectorXd step(const double &dt, const Eigen::VectorXd &flow_bnd,
                         const Eigen::VectorXd &sediment_bnd,
                         const Eigen::VectorXd &settle_params,
                         Ammonification &sys_ammonification,
                         Nitrification &sys_nitrification,
                         Denitrification &sys_denitrification);

    /* @brief 给定一个水库状态向前推进dt,并返回新的水库状态
     * @param s0 给定一个水库的状态, 格式如下(8)
     * [water_level, c_rpon, c_lpon, c_don, c_ammonia, c_nitrate,
     * temperature, c_oxygen]
     * @param dt 推进时间步长,单位s
     * @param flow_bnd 当前时刻流动边界条件，格式如下(12个)：
     * [flow_in, load_rpon_in, load_lpon_in, load_don_in, load_amm_in,
     * load_nit_in, flow_out, load_rpon_out, load_lpon_out, load_don_out,
     * load_amm_out, load_nit_out]
     * @param sediment_bnd 当前时刻的底泥边界条件，格式如下(3个)：
     * [bndo, bna, bnn], 分别为溶解性有机氮，氨氮和硝氮的释放量
     * @param settle_params 颗粒有机氮的沉降参数，存储格式为：
     * [s_nrp, s_nlp]
     * @param Ammonification 氨化系统
     * @param Nitrification 硝化系统
     * @param Denitrification 反硝化系统
     */
    Eigen::VectorXd predict(const Eigen::VectorXd &s0, const double &dt,
                            const Eigen::VectorXd &flow_bnd,
                            const Eigen::VectorXd &sediment_bnd,
                            const Eigen::VectorXd &settle_params,
                            Ammonification &sys_ammonification,
                            Nitrification &sys_nitrification,
                            Denitrification &sys_denitrification);
};
#endif