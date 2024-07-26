/*!
 * @file ReservoirSystem.h
 * @brief 用于水库系统水质和水位的预测
 * @author xfwahss
 * @version main-v0.2
 * @date 2024-07-25
 */
#include <Ammonification.h>
#include <Denitrification.h>
#include <Nitrification.h>
#include <ParticleSettle.h>
#include <RiverGroup.h>
#include <Sediment.h>
#include <WaterColumn.h>
#include <functional>

class ReservoirSystem {
  private:
    static RiverGroup in;
    static RiverGroup out;
    static WaterColumn res;
    static Ammonification ammon;
    static Nitrification nitri;
    static Denitrification deni;
    static Sediment sedi;
    static ParticleSettle particles;

    static bool system_inited;
    static int status_len;
    static int bnd_force_len;
    static int parameters_len;
    static int in_status_len;
    static int out_status_len;
    static int ammonification_len;
    static int nitrification_len;
    static int denitrification_len;
    static int sediment_len;
    static int particlesettle_len;
    static void update_status(const Eigen::VectorXd &status);
    static void update_bnd(const Eigen::VectorXd &bnd_force);
    static void update_params(const Eigen::VectorXd &params);

  public:
    /*
     *@breif 初始化水库系统
     *@param: in_nums 入库河流数目
     *@param: out_nums 出库河流数目
     */
    static void init_system(const int &in_nums, const int &out_nums);
    static void set_wl_volumn_function(
        std::function<double(const double &wl)> wl_to_volumn,
        std::function<double(const double &wl)> wl_to_depth,
        std::function<double(const double &volumn)> volumn_to_wl);
    // 给定一个状态和，并由当前状态向前推进一个时间步dt, 返回递推的状态结果
    /* @brief 给定一个水库的状态和相关边界条件及水质参数，向前递推一个时间步
     * @param status 水库当前的状态,存储格式为:
     * [water_level, c_rpon, c_lpon, c_don, c_na, c_nn, temperature, oxygen]
     * @param bnd_force 水库的边界条件(流量m3/s,浓度 g/m3)，存储格式为
     * [flow, c_rpon, c_lpon, c_don, c_na, c_nn] * in_nums +
     * [flow, c_rpon, c_lpon, c_don, c_na, c_nn] * out_nums
     * @param params 水库各系统参数(反应速率都是以d为单位)，存储格式为
     * [k_rpon_20, theta_rpon, k_lpon_20, theta_lpon, k_don_20, theta_don,++
     *  r_nit0,k_nit_20,foxmin,c_oxc_nit,c_oxo_nit,thetat_nit,T_c_nit,alpha,++
     *  r_deni0,k_deni_20,Tc_deni,theta_deni,C_oxc_deni,c_oxo_deni,beta,++
     *  don_flux, ammonia_flux, nitrate_flux,++
     *  s_nrp, s_nlp
     * ]
     * @param dt 递推时间步，单位为s
     */
    static Eigen::VectorXd predict(const Eigen::VectorXd &status,
                                   const Eigen::VectorXd &bnd_force,
                                   const Eigen::VectorXd &params,
                                   const double &dt);
};