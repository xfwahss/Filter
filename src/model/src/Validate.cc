#include "../../filter/include/EnsembleKalmanFilter.h"
#include "../../io/include/ModelIO.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model {
  private:
    Eigen::VectorXd status;

  public:
    Eigen::VectorXd get_status() { return status; }
    void update(Eigen::VectorXd &status) { this->status = status; }
    void predict(const double &dt) {
        // for (int i = 0; i < status.size(); ++i) {
        //     status(i) += dt;
        // }
        // status 不变
    }
};

int main() {

    EnsembleKalmanFilter<Model> enkal(2000);
    ModelIO modelio("../data/TestModelIO_in.xlsx",
                    "../data/TestModelIO_out.xlsx");
    enkal.init(modelio.get_init_X(), modelio.get_init_P(), modelio.get_H(),
               modelio.get_Q());
    enkal.batch_assimilation(&modelio, 1);
}