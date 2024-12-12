#include <EnsembleKalmanFilter.h>
#include <algorithm>
#include <args.hxx>
#include <config.h>
#include <string>

// 负责粒子更新步, 将状态向量分配给机理模型
class Model : public EnsembleModel {
  private:
    double lnsqrt(double &&x) { return std::log(x + std::sqrt(1 + x * x)); }

  public:
    static double alpha;
    static double amplitude;
    double relative_error = 0;
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        Eigen::VectorXd s(status.size());
        relative_error = get_diff_obs_prior()[0]; // 读取相对误差
        if (std::abs(relative_error) > alpha) {
            s(3)        = -lnsqrt(std::move(relative_error)) / dt;
            double prev = status(3) * (1 - amplitude);
            s(2)        = status(2) * amplitude * std::exp(-amplitude * dt) + (s(3) - prev) * amplitude;
        } else {
            s(3) = 0;
            s(2) = status(2) * amplitude * std::exp(-amplitude * dt);
        }
        s(1) = std::max(status(1) + s(2) * dt, 0.0);
        s(0) = status(0) * std::exp(-s(1) * dt);
        return s;
    }
};
double Model::alpha     = 0.2;
double Model::amplitude = 0.10;

int main(int argc, char **argv) {
    if (std::string(DEBUG_LEVEL) == std::string("DEBUG")) {
        // logger::get("Ensemble RE")->set_level(spdlog::level::debug);
        // logger::get("umath")->set_level(spdlog::level::debug);
        // logger::get()->set_level(spdlog::level::debug);
        // Logger::set_level("const std::string &name", "debug");
        // std::cout << Logger::logger_set[0] << std::endl;
    }
    args::ArgumentParser parser("1stOrderReactionParameterTracker", "END");
    args::ValueFlag<std::string> fi(parser, "filein", "file input", {'i'});
    args::ValueFlag<std::string> fo(parser, "fileout", "file output", {'o'});
    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
    }
    std::string filename_in, filename_out;
    if (fi) {
        filename_in = args::get(fi);
    }
    if (fo) {
        filename_out = args::get(fo);
    }

    FilterIO modelio(filename_in, filename_out);
    std::unordered_map<std::string, double> params = modelio.get_params();
    int ensemble_size                              = params["size"];
    double dt                                      = params["dt"];
    int status_dim                                 = params["status_dims"];
    int obs_dim                                    = params["obs_dims"];
    double get_tau                                 = params["tau"];
    double get_amplitude                           = params["amplitude"];

    Model::alpha                                   = get_tau;
    Model::amplitude                               = get_amplitude;
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