#include "../../filter/include/EnsembleKalmanFilter.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model : public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        std::cout<<"current status:" <<status(0) << ", " << status(1) <<std::endl;
        Eigen::VectorXd s(status.size());
        s(0) = status(0) - status(1) * status(0) * dt;
        s(1) = status(1);
        std::cout<<"predict status:" << status(0) << ", "<< s(1) << std::endl;
        return s;
    }
};

int main() {

    FilterIO modelio("../test/data/Degradation_in.xlsx",
                     "../test/data/Degradation_out.xlsx");
    std::unordered_map<std::string, double> params = modelio.get_params();
    int ensemble_size                              = params["size"];
    double dt                                      = params["dt"];
    EnsembleKalmanFilter<Model> enkal(ensemble_size);
    std::cout << "ensemble_size:" << ensemble_size << std::endl;
    std::cout << "dt:" << dt << std::endl;
    Eigen::MatrixXd P = modelio.get_init_P("Init_P", 1, 1, 2);
    Eigen::VectorXd X = modelio.get_init_X("Init_X", 2, 1, 2);
    Eigen::MatrixXd H = modelio.get_H("H", 1, 1, 1, 2);
    Eigen::VectorXd Q = modelio.get_Q("Q", 2, 1, 2);
    std::cout << "Init P:" << "\n" << P << "\n" << "**********" << "\n";
    std::cout << "Init X:" << "\n" << X << "\n" << "**********" << "\n";
    std::cout << "H:" << "\n" << H << "\n" << "**********" << "\n";
    std::cout << "Q:" << "\n" << Q << "\n" << "**********" << "\n";
    enkal.init(X, P, H, Q);
    enkal.batch_assimilation(&modelio, dt);
}