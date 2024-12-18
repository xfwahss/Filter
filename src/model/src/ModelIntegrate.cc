#include <EnsembleKalmanFilter.h>
#include <FilterIO.h>
#include <ReservoirSystem.h>
#include <args.hxx>

class ModelIO : public FilterIO {
  private:
    // 前向填充读取的数据
    void fill_forward(Eigen::MatrixXd &m) {
        for (int i = 1; i < m.rows(); i++) {
            for (int j = 0; j < m.cols(); j++) {
                if (m(i, j) == -999) {
                    m(i, j) = m(i - 1, j);
                }
            }
        }
    };

    // 将读取的河流数据转化为0维水质模型的格式
    void get_bnd_force(Eigen::MatrixXd &m) {
        for (int i = 0; i < m.rows(); i++) {
            Eigen::RowVectorXd merged(20);
            merged << RiversInData.row(i), RiversOutData.row(i);
            for (int j = 0; j < 5; j++) {
                m(i, j * 6 + 0) = merged(j * 4 + 0);
                m(i, j * 6 + 1) =
                    (merged(j * 4 + 1) - merged(j * 4 + 2) - merged(j * 4 + 3)) * params_dict["rpon_ratio"];
                m(i, j * 6 + 2) =
                    (merged(j * 4 + 1) - merged(j * 4 + 2) - merged(j * 4 + 3)) * params_dict["lpon_ratio"];
                m(i, j * 6 + 3) = (merged(j * 4 + 1) - merged(j * 4 + 2) - merged(j * 4 + 3)) *
                                  (1 - params_dict["lpon_ratio"] - params_dict["rpon_ratio"]);
                m(i, j * 6 + 4) = merged(j * 4 + 2);
                m(i, j * 6 + 5) = merged(j * 4 + 3);
            }
        }
    }

    // 将读取的水库水质数据转化成水质水质模型输出的格式
    void transform_data() {
        Eigen::MatrixXd m(ReservoirData.rows(), ReservoirData.cols() + 2);
        for (int i = 0; i < m.rows(); i++) {
            Eigen::RowVectorXd newm(m.cols());
            newm(0) = ReservoirData(i, 0);
            if (ReservoirData(i, 1) == -999.0) {
                newm(1) = -999.0;
                newm(2) = -999.0;
                newm(3) = -999.0;
            } else {
                newm(1) = ReservoirData(i, 1) * params_dict["rpon_ratio"];
                newm(2) = ReservoirData(i, 1) * params_dict["lpon_ratio"];
                newm(3) = ReservoirData(i, 1) - newm(1) - newm(2);
            }
            newm(4)  = ReservoirData(i, 2);
            newm(5)  = ReservoirData(i, 3);
            newm(6)  = ReservoirData(i, 4);
            newm(7)  = ReservoirData(i, 5);
            m.row(i) = newm;
        }
        ReservoirData = m;
    }

  public:
    std::unordered_map<std::string, double> params_dict; // 同化模型参数
    Eigen::MatrixXd P;
    Eigen::VectorXd X;
    Eigen::MatrixXd H;
    Eigen::VectorXd Q;
    Eigen::MatrixXd R;
    Eigen::MatrixXd RiversInData;
    Eigen::MatrixXd RiversOutData;
    Eigen::MatrixXd ReservoirData;
    Eigen::VectorXd model_params;
    Eigen::MatrixXd BndForce;
    Eigen::MatrixXd Updated_X;
    Eigen::MatrixXd Updated_P;
    int index = 0;
    ModelIO(std::string &filename_in, std::string &filename_out, int mode = -1) : FilterIO(filename_in, filename_out) {
        logger::get()->info("Reading data from: {}", filename_in);
        params_dict = FilterIO::get_params("Params", 1, 2, 1);
        if (mode == -1) {
            mode = params_dict["mode"];
        } else {
            params_dict["mode"] = mode;
        }
        int status_dims;
        Logger::get()->info("Current operation mode is: {}", params_dict["mode"]);
        switch (mode) {
        case 1:
            status_dims = params_dict["wq_status_num"];
            break;
        case 2:
            status_dims = params_dict["wq_status_num"] + params_dict["param_status_num"];
            break;
        case 3:
            status_dims = params_dict["wq_status_num"] + params_dict["param_status_num"];
            break;
        default:
            break;
        }
        params_dict["status_dims"] = status_dims;
        P                          = FilterIO::get_init_P("Init_P", 2, 2, params_dict["status_dims"]);
        X                          = FilterIO::get_init_X("Init_X", 2, 1, params_dict["status_dims"]);
        H                          = FilterIO::get_H("H", 2, 2, params_dict["obs_dims"], params_dict["status_dims"]);
        Q                          = FilterIO::get_Q("Q", 2, 1, params_dict["status_dims"]);
        R                          = FilterIO::get_init_P("R", 2, 2, params_dict["obs_dims"]);
        model_params               = FilterIO::get_init_X("ModelParams", 2, 1, params_dict["model_params_num"]);
        RiversInData               = FilterIO::get_matrix("Rivers_in", 2, 2, params_dict["measurement_nums"],
                                                          params_dict["riversin_nums"] * params_dict["river_monitor_num"]);
        RiversOutData              = FilterIO::get_matrix("Rivers_out", 2, 2, params_dict["measurement_nums"],
                                                          params_dict["riversout_nums"] * params_dict["river_monitor_num"]);
        ReservoirData =
            FilterIO::get_matrix("ResAvg", 2, 2, params_dict["measurement_nums"], params_dict["reservoir_col"]);
        this->fill_forward(RiversInData);
        this->fill_forward(RiversOutData);
        // this->fill_forward(ReservoirData); // 水库数据不能填充，是用来同化的
        BndForce = Eigen::MatrixXd(static_cast<int>(params_dict["measurement_nums"]), 30);
        get_bnd_force(BndForce);
        this->transform_data(); // 将水库观测数据转化为同化格式
        Updated_X = Eigen::MatrixXd::Zero(params_dict["measurement_nums"], params_dict["status_dims"]);
        Updated_P = Eigen::MatrixXd::Zero(params_dict["measurement_nums"], params_dict["status_dims"]);
    };
    void write_X() {
        FilterIO::write_headers();
        for (int i = 0; i < Updated_X.rows(); i++) {
            FilterIO::write_x(Updated_X.row(i), "X", i + 2, 2);
        }
    }
    void write_P() {
        for (int i = 0; i < Updated_P.rows(); i++) {
            FilterIO::write_x(Updated_P.row(i), "P", i + 2, 2);
        }
    }
    ~ModelIO(){};
};

class MechanismUpdate {
  private:
    static double wl_to_volume(const double &wl) { return 0.022846 * wl * wl - 5.341818 * wl + 314.495276; }
    static double wl_to_depth(const double &wl, const double &h0) { return wl - h0; }
    static double volume_to_wl(const double &volume) {
        double a = 0.022846;
        double b = -5.341818;
        double c = 314.495276 - volume;
        double x = (-b + std::sqrt(b * b - 4 * a * c)) / a / 2;
        return x;
    }

  public:
    ModelIO mio;
    MechanismUpdate(std::string &filename_in, std::string &filename_out, int mode = -1)
        : mio(filename_in, filename_out, mode) {
        ReservoirSystem::init_system(mio.params_dict["riversin_nums"], mio.params_dict["riversout_nums"]);
        double h0                                             = mio.params_dict["h0"];
        std::function<double(const double &)> wl_to_depth_set = [this, &h0](const double &wl) -> double {
            return this->wl_to_depth(wl, h0);
        };
        ReservoirSystem::set_wl_volumn_function(this->wl_to_volume, wl_to_depth_set, this->volume_to_wl);
    };
    ~MechanismUpdate(){};
    Eigen::VectorXd predict(const Eigen::VectorXd &status, const Eigen::VectorXd &bnd_force,
                            const Eigen::VectorXd &params, const double &dt) {
        return ReservoirSystem::predict(status, bnd_force, params, dt);
    };
};

class Model : public EnsembleModel {
  public:
    static MechanismUpdate *mu;
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        // std::cout << "Before:  " << status.transpose() << std::endl;
        int mode = mu->mio.params_dict["mode"];
        Eigen::VectorXd next_status(status.size());
        Eigen::VectorXd wq_status, param_status, next_wq, next_param; // 只有在同化水质和参数时才使用
        switch (mode) {
        case 1:
            // 仅同化水质状态
            next_status = mu->predict(status, mu->mio.BndForce.row(mu->mio.index), mu->mio.model_params, dt);
            for (int i = 0; i < next_status.size(); i++) {
                next_status(i) = std::max(next_status(i), 0.0);
            }
            break;
        case 2:
            // 同化水质状态和水质参数
            wq_status = status.segment(0, mu->mio.params_dict["wq_status_num"]);
            param_status =
                status.segment(mu->mio.params_dict["wq_status_num"], mu->mio.params_dict["param_status_num"]);
            next_wq    = mu->predict(wq_status, mu->mio.BndForce.row(mu->mio.index), param_status, dt);
            next_param = param_status;
            next_status << next_wq, next_param;
            break;
        case 3:
            // 基于刺激-响应的参数同化框架
            break;
        default:
            throw std::logic_error("Wrong mode, please select 1, 2, or 3");
            break;
        }
        // std::cout << "After:  " << next_status.transpose() << std::endl;
        return next_status;
    };
};
MechanismUpdate *Model::mu = nullptr;

int main(int argc, char *argv[]) {
    std::string filename_in, filename_out;
    int mode;
    args::ArgumentParser parser("1stOrderReactionParameterTracker", "END");
    args::ValueFlag<std::string> fi(parser, "filein", "file input", {'i'});
    args::ValueFlag<std::string> fo(parser, "fileout", "file output", {'o'});
    args::ValueFlag<std::string> mode_p(parser, "mode", "mode selection", {'m'});
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
    if (fi) {
        filename_in = args::get(fi);
    }
    if (fo) {
        filename_out = args::get(fo);
    }
    if (mode_p) {
        mode = std::stoi(std::string(args::get(mode_p)));
    }

    MechanismUpdate mu(filename_in, filename_out, mode);
    if (mode != 0) {
        Model::mu = &mu;
        EnsembleKalmanFilter<Model> enf(mu.mio.params_dict["size"]);
        bool enable_pos;
        if (mu.mio.params_dict["enable_pos"] == 1) {
            enable_pos = true;
        } else {
            enable_pos = false;
        }
        enf.init(mu.mio.X, mu.mio.P, mu.mio.H, mu.mio.Q, 0, enable_pos);
        for (int i = 0; i < mu.mio.params_dict["measurement_nums"]; i++) {
            Logger::get("console")->info("processing index: {}", i);
            mu.mio.index = i;
            enf.step_assimilation(mu.mio.params_dict["dt"] * 86400, mu.mio.ReservoirData.row(mu.mio.index), mu.mio.R);
            mu.mio.Updated_X.row(i) = enf.status().transpose();
            for (int j = 0; j < mu.mio.X.size(); j++) {
                mu.mio.Updated_P(i, j) = enf.covariance()(j, j);
            }
        }
        mu.mio.write_X();
        mu.mio.write_P();
    } else {
        Eigen::VectorXd cur = mu.mio.ReservoirData.row(0);
        for (int i = 0; i < mu.mio.params_dict["measurement_nums"]; i++) {
            mu.mio.Updated_X.row(i) = cur;
            Logger::get("console")->info("processing index: {}", i);
            std::cout << mu.mio.BndForce.row(i) << std::endl;
            cur = mu.predict(cur, mu.mio.BndForce.row(i), mu.mio.model_params, mu.mio.params_dict["dt"] * 86400);
        }
        mu.mio.write_X();
    }
    return 0;
}
