#include "EnsembleKalmanFilter.h"
#include "config.h"

// 负责粒子更新步, 将状态向量分配给机理模型
class Model : public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        // std::cout<<"current status:" <<status <<std::endl;
        Eigen::VectorXd s(status.size());
        s(0) = status(0) * std::exp(- status(1) * dt);
        // Eigen::VectorXd k = const_2derivate_predict(status.segment(1, 3), dt);
        // s(1) = k(0);
        // s(2) = k(1);
        // s(3) = k(2);


        // 类似生态系统的模拟
        Eigen::VectorXd diff = get_diff_obs_prior();
        s(1) = status(1) + status(2) * dt;
        // 换成没有更新就衰减，有更新直接用新的？
        s(2) = (status(2) + status(3)) * std::exp(-1);
        if(diff(0) > 0.10){
            // 应该按照k的0.2k的增幅，不能太离谱
            double k = status(1);
            double re = diff(0);
            s(3) =  std::log(1 - re) / dt / dt * 0.050;
        } else{
            s(3) = 0;
        }
        // std::cout<<"predict status:" << status(0) << ", "<< s(1) << std::endl;
        return s;
    }
};

int main() {
    std::cout << DEBUG_LEVEL;
    spdlog::set_level(spdlog::level::debug);

    FilterIO modelio("../test/data/Degradation_in.xlsx",
                     "../test/output/Degradation_out.xlsx");
    std::unordered_map<std::string, double> params = modelio.get_params();
    int ensemble_size                              = params["size"];
    double dt                                      = params["dt"];
    int status_dim = params["status_dims"];
    int obs_dim = params["obs_dims"];
    EnsembleKalmanFilter<Model> enkal(ensemble_size);
    std::cout << "ensemble_size:" << ensemble_size << std::endl;
    std::cout << "dt:" << dt << std::endl;
    Eigen::MatrixXd P = modelio.get_init_P("Init_P", 1, 1, status_dim);
    Eigen::VectorXd X = modelio.get_init_X("Init_X", 2, 1, status_dim);
    Eigen::MatrixXd H = modelio.get_H("H", 1, 1, obs_dim, status_dim);
    Eigen::VectorXd Q = modelio.get_Q("Q", 2, 1, status_dim);
    std::cout << "Init P:" << "\n" << P << "\n" << "**********" << "\n";
    std::cout << "Init X:" << "\n" << X << "\n" << "**********" << "\n";
    std::cout << "H:" << "\n" << H << "\n" << "**********" << "\n";
    std::cout << "Q:" << "\n" << Q << "\n" << "**********" << "\n";
    enkal.init(X, P, H, Q);
    enkal.batch_assimilation(&modelio, dt);
}