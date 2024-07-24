#include <EnsembleKalmanFilter.h>
#include <config.h>
#include <string>

// 负责粒子更新步, 将状态向量分配给机理模型
class Model : public EnsembleModel {
  public:
    static double tau;
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        Eigen::VectorXd s(status.size());
        s(0)                 = status(0) * std::exp(-status(1) * dt / 10);
        // Eigen::VectorXd k = const_2derivate_predict(status.segment(1, 3),
        // dt); s(1) = k(0); s(2) = k(1); s(3) = k(2);

        // 类似生态系统的模拟
        Eigen::VectorXd diff = get_diff_obs_prior();
        s(1)                 = status(1) + status(2) * dt;
        s(2)                 = status(2) * std::exp(-std::abs(status(2)) / tau * dt) + status(3);
        if (diff(0) > tau || diff(0) < (-tau)) {
            double re = diff(0);
            double amplitude;
            amplitude = - tau * std::log(re + std::sqrt(1 + re * re)) / dt;
            // sigmoid 函数变体
            // amplitude = -tau * 2 * (1 / (1 + std::exp(-re)) - 0.5);
            s(3)      = amplitude;
        } else {
            s(3) = 0;
        }
        if (s(3) * s(2) < 0) {
            s(2) = s(3);
        }
        return s;
    }
};
double Model::tau = 0.2;

int main() {
    if (std::string(DEBUG_LEVEL) == std::string("DEBUG")) {
        // logger::get("Ensemble RE")->set_level(spdlog::level::debug);
        // logger::get("umath")->set_level(spdlog::level::debug);
        // logger::get()->set_level(spdlog::level::debug);
        // Logger::set_level("const std::string &name", "debug");
        // std::cout << Logger::logger_set[0] << std::endl;
    }

    FilterIO modelio("../test/data/Degradation_in.xlsx", "../test/output/Degradation_out.xlsx");
    std::unordered_map<std::string, double> params = modelio.get_params();
    int ensemble_size                              = params["size"];
    double dt                                      = params["dt"];
    int status_dim                                 = params["status_dims"];
    int obs_dim                                    = params["obs_dims"];
    double get_tau                                 = params["tau"];
    Model::tau                                     = get_tau;
    EnsembleKalmanFilter<Model> enkal(ensemble_size);
    spdlog::info("The size of ensemble is: {}", ensemble_size);
    spdlog::info("dt is: {}", dt);
    Eigen::MatrixXd P = modelio.get_init_P("Init_P", 1, 1, status_dim);
    Eigen::VectorXd X = modelio.get_init_X("Init_X", 2, 1, status_dim);
    Eigen::MatrixXd H = modelio.get_H("H", 1, 1, obs_dim, status_dim);
    Eigen::VectorXd Q = modelio.get_Q("Q", 2, 1, status_dim);

    std::stringstream ss;
    ss << "\n" << P;
    spdlog::info("Init P: {}", ss.str());

    ss.str("");
    ss << X.transpose();
    spdlog::info("Init X: {}", ss.str());

    ss.str("");
    ss << H;
    spdlog::info("H: {}", ss.str());

    ss.str("");
    ss << Q.transpose();
    spdlog::info("Q: {}", ss.str());
    enkal.init(X, P, H, Q, 30);
    enkal.batch_assimilation(&modelio, dt);
}