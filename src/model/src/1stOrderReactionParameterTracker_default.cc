#include <EnsembleKalmanFilter.h>
#include <args.hxx>
#include <config.h>
#include <string>

// 负责粒子更新步, 将状态向量分配给机理模型
class Model : public EnsembleModel {
  public:
    static double tau;
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        Eigen::VectorXd s(status.size());
        s(0) = status(0) * std::exp(-status(1) * dt);
        s(1) = status(1);
        s(2) = status(2);
        s(3) = status(3);
        return s;
    }
};
double Model::tau = 0.2;

int main(int argc, char **argv) {
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