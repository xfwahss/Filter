#include "../../filter/include/EnsembleKalmanFilter.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model: public EnsembleModel {
  public:
    void predict(const double &dt) {
      Eigen::VectorXd s = get_status();
      s = const_1derivate_predict(s, dt);
      update(s);
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