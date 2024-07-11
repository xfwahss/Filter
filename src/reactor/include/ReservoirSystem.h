/*!
 * @file ReservoirSystem.h
 * @brief 用于水库系统水质和水位的预测
 * @author xfwahss
 * @version main-v0.1
 * @date 2024-07-10
 */
#include <DataStructures.h>
#include <Ammonification.h>
#include <Denitrification.h>
#include <Eigen/Dense>
#include <Nitrification.h>
#include <Reservoir.h>
#include <RiverGroup.h>

class ReservoirSystem {
  private:
    static RiverGroup in;
    static RiverGroup out;
    static Reservoir res;
    static Ammonification ammon;
    static Nitrification nitri;
    static Denitrification deni;

    static bool system_inited;
    static int status_len;
    static int bnd_force_len;
    static int in_status_len;
    static int out_status_len;
    static int ammonification_len;
    static int nitrification_len;
    static int denitrification_len;

    /*
     *@breif 初始化水库系统
     *@param: bnd_force 水库边界条件：[入库河流状态， 出库河流状态， 氨化系统参数， 硝化系统参数， 反硝化系统参数]
     *@param: in_nums 入库河流数目
     *@param: out_nums 出库河流数目
     */
    static void init_system(const Eigen::VectorXd &bnd_force, const int &in_nums, const int &out_nums);
    static void update_bnd(const Eigen::VectorXd &bnd_force, const int &in_nums, const int &out_nums);
    static void update_status(const Eigen::VectorXd &status);

  public:
    // 给定一个状态和，并由当前状态向前推进一个时间步dt, 返回递推的状态结果
    static Eigen::VectorXd &predict(Eigen::VectorXd &status, const Eigen::VectorXd &bnd_force, const int &in_nums,
                                    const int &out_nums, const double &dt);
};