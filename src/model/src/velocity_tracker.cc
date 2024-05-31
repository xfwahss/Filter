#include "../../filter/include/EnsembleKalmanFilter.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model: public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
      Eigen::VectorXd s(status.size());
      s(0) = status(0) + status(1) * dt;
      Eigen::VectorXd s2 = const_2derivate_predict(status.segment(1, 3), dt);
      s(1) = s2(0);
      s(2) = s2(1);
      s(3) = s2(2);
      return s;
    }
};

int main() {

    EnsembleKalmanFilter<Model> enkal(2000);
    FilterIO modelio("../data/velocity_tracker_in.xlsx",
                    "../data/velocity_tracker_out.xlsx");
    enkal.init(modelio.get_init_X(), modelio.get_init_P(), modelio.get_H(),
               modelio.get_Q());
    enkal.batch_assimilation(&modelio, 0.1);
}