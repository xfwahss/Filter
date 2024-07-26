#ifndef PARTICLESETTLE_H
#define PARTICLESETTLE_H
#include <Eigen/Dense>

class ParticleSettle {
  private:
    double s_nrp;
    double s_nlp;

  public:
    static const int param_nums;
    void init(const double &s_nrp, const double &s_nlp);
    /* @brief 更新底泥沉降信息
     * @param vectorxd s, s的数据存储格式为:
     * [s_nrp, s_nlp]
     */
    void update(const Eigen::VectorXd &s);
    Eigen::VectorXd get_settle_params();
};

#endif