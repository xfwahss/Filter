#include "../../filter/include/EnsembleKalmanFilter.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model: public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
      Eigen::VectorXd s = const_1derivate_predict(status, dt);
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