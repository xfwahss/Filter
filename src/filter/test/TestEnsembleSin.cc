#include "../include/EnsembleKalmanFilter.h"

class Model : public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        return EnsembleModel::const_2derivate_predict(status, dt);
    }
};

class ModelIO : public FilterIO {
  public:
    ModelIO(const std::string &filename_in, const std::string &filename_out)
        : FilterIO(filename_in, filename_out){};
    ~ModelIO(){};
    void read_one(ExcelIO &file, Eigen::VectorXd &z, Eigen::MatrixXd &R,
                  const int &index) {
    int obs_dims = get_params("Params")["obs_dims"];
    z = file.read_row("Z", index+2, 2);
    R = file.read_block("R", 2, 2, obs_dims, obs_dims);
    }
};

int main() {
    std::string filename_in  = "../test/data/sin.xlsx";
    std::string filename_out = "../test/data/filter_sin.xlsx";

    ModelIO modelio(filename_in, filename_out);
    std::unordered_map<std::string, double> params = modelio.get_params();
    EnsembleKalmanFilter<Model> enkal(params["size"]);
    int status_dims   = params["status_dims"];
    int obs_dims      = params["obs_dims"];
    Eigen::VectorXd X = modelio.get_init_X("Init_X", 2, 1, status_dims);
    Eigen::MatrixXd P = modelio.get_init_P("Init_P", 2, 2, status_dims);
    Eigen::MatrixXd H = modelio.get_H("H", 2, 2, obs_dims, status_dims);
    Eigen::VectorXd Q = modelio.get_Q("Q", 2, 1, status_dims);
    enkal.init(X, P, H, Q);
    enkal.batch_assimilation(&modelio, 1);
}