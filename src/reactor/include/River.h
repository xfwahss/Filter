#ifndef RIVER_H
#define RIVER_H
#include <Eigen/Dense>

class River {
  private:
    double flow;
    double c_rpon;
    double c_lpon;
    double c_don;
    double c_na;
    double c_nn;

  public:
    static const int status_nums;
    River();
    ~River();
    void init(const double &flow, const double &c_rpon, const double &c_lpon,
              const double &c_don, const double &c_na, const double &c_nn);
    /* @brief 更新河流状态信息
     * @param s 输入vector的变量格式如下：
     * [flow, c_rpon, c_lpon, c_don, c_na, c_nn]
     */
    void update(const Eigen::VectorXd &s);
    double flow_rate();
    double load_rpon();
    double load_lpon();
    double load_don();
    double load_na();
    double load_nn();
    /* @brief 返回河流的流量及相关负荷
     *  @return Eigen::VectorXd 格式如下：
     *  [flow, load_rpon, load_lpon, load_don, load_na, load_cnn]
     */
    Eigen::VectorXd flow_status();
};
#endif