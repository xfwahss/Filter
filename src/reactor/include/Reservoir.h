#ifndef RESERVOIR_H
#define RESERVOIR_H
#include "DataStructures.h"

class Reservoir {
  private:
    res_status status;
    // 水量单位10^8 m3
    double volumn;
    // 由水位计算出水量
    double wl_to_volumn(const double &wl);
    // 由水量计算出水位
    double volumn_to_wl(const double &volumn);
    double predict_volumn(const double &seconds, const double &flow_in, const double &flow_out);
    /*
     * @brief 计算下一时间步的有机氮浓度
     * @param seconds 时间秒
     * @param load_in 有机氮入库负荷 g/s
     * @param load_out 有机氮出库负荷 g/s
     * @param ro 氨化速率 g/m3/s
     * @param next_volumn 下一时刻水量 1e-8 m3, 由predict_volumn估算
     */
    double predict_organic(const double &seconds, const double &load_in, const double &load_out, const double &ro,
                           const double &next_volumn);
    /*
     * @brief 计算下一时间步的氨氮浓度
     * @param seconds 时间秒
     * @param load_in 氨氮入库负荷 g/s
     * @param load_out 氨氮出库负荷 g/s
     * @param ro 氨化速率 g/m3/s
     * @param ra 硝化速率 g/m3/s
     * @param next_volumn 下一时刻水量 1e-8 m3, 由predict_volumn估算
     */
    double predict_ammonia(const double &seconds, const double &load_in, const double &load_out, const double &ro,
                           const double &ra, const double &next_volumn);
    /*
     * @brief 计算下一时间步的硝氮浓度
     * @param seconds 时间秒
     * @param load_in 硝氮入库负荷 g/s
     * @param load_out 硝氮出库负荷 g/s
     * @param ra 硝化速率 g/m3/s
     * @param rn 反硝化速率 g/m3/s
     * @param next_volumn 下一时刻水量 1e-8 m3, 由predict_volumn估算
     */
    double predict_nitrate(const double &seconds, const double &load_in, const double &load_out, const double &ra,
                           const double &rn, const double &next_volumn);

  public:
    Reservoir();
    ~Reservoir();
    void init(const double &wl, const double &c_no, const double &c_na, const double &c_nn, const double &T,
              const double &c_do);
    res_status get_status();
    // 更新系统当前先验状态信息
    void update(res_status &status);
    void update(const Eigen::VectorXd &status);
    /*
     * @brief 由当前状态向前推进一个时间步
     * @param dt 天
     * @param flow_in 入库流量
     * @param flow_out 出库流量
     * @param load_org_in 入库有机氮
     * @param load_org_out 出库有机氮
     * @param load_amm_in 入库氨氮
     * @param load_amm_out 出库氨氮
     * @param load_nit_in 入库硝氮
     * @param load_nit_out 出库硝氮
     * @param ro 有机氮反应速率
     * @param ra 氨氮反应速率
     * @param rn 硝态氮反应速率
     */
    void predict(const double &dt, const double &flow_in, const double &flow_out, const double &load_org_in,
                 const double &load_org_out, const double &load_amm_in, const double &load_amm_out,
                 const double &load_nit_in, const double &load_nit_out, const double &ro, const double &ra,
                 const double &rn);
    Eigen::VectorXd predict(const Eigen::VectorXd &flow_in, const Eigen::VectorXd &flow_out, const double &ro,
                             const double &ra, const double &rn, const double &dt);
};
#endif