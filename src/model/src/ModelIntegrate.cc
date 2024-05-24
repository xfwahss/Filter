#include "../../filter/include/EnsembleKalmanFilter.h"
#include "../../reactor/include/Denitrification.h"
#include "../../reactor/include/Nitrification.h"
#include "../../reactor/include/Reservoir.h"
#include "../../reactor/include/RiverSystem.h"
#include "../../utils/commands.h"

class Model : public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) {
        Reservoir reservoir;
        Denitrification deni_proc;
        Nitrification ni_proc;
        River baihe;
        River chaohe;
        River bai_dam;
        River chao_dam;
        RiverSystem rivers_in;
        RiverSystem rivers_out;
        rivers_in.add_river("Baihe", baihe);
        rivers_in.add_river("Chaohe", baihe);
        rivers_out.add_river("Baihe_Dam", bai_dam);
        rivers_out.add_river("Chaohe_Dam", chao_dam);
        // 粒子状态向量分发
        double in_flow1     = status(0);
        double in_flow1_cno = 0;
        double in_flow1_cna = 0;
        double in_flow1_cnn = 0;

        double in_flow2     = status(1);
        double in_flow2_cno = 0;
        double in_flow2_cna = 0;
        double in_flow2_cnn = 0;

        double out_flow1     = status(2);
        double out_flow1_cno = 0;
        double out_flow1_cna = 0;
        double out_flow1_cnn = 0;

        double out_flow2     = status(3);
        double out_flow2_cno = 0;
        double out_flow2_cna = 0;
        double out_flow2_cnn = 0;

        double wl      = status(4);
        double res_cno = 0;
        double res_cna = 0;
        double res_cnn = 0;
        double res_T   = 0;
        double res_cdo = 0;

        double deni_rn0   = 0; // 0阶反应速率
        double deni_knb1  = 0; // 20度时的反硝化速率常数
        double deni_Tnc   = 0; // 临界温度
        double deni_theta = 0; // 温度系数
        double deni_cnoxc = 0; // 临界溶解氧浓度
        double deni_cnoxo = 0; // 最佳溶解氧浓度
        double beta       = 1; // 曲率系数

        double ni_ra0     = 0;
        double ni_kab1    = 0;
        double ni_foxmin  = 0;
        double ni_c_oxc   = 0;
        double ni_c_oxo   = 0;
        double ni_theta_a = 0;
        double ni_T_c     = 0;
        double alpha      = 0;

        // 子模型状态更新
        res_status res_s(wl, res_cno, res_cna, res_cnn, res_T, res_cdo);
        reservoir.update(res_s);

        Eigen::VectorXd rivers_in_s(8);
        rivers_in_s << in_flow1, in_flow1_cno, in_flow1_cna, in_flow1_cnn,
            in_flow2, in_flow2_cno, in_flow2_cna, in_flow2_cnn;
        rivers_in.update(rivers_in_s);

        Eigen::VectorXd rivers_out_s(8);
        rivers_out_s << out_flow1, out_flow1_cno, out_flow1_cna, out_flow1_cnn,
            out_flow2, out_flow2_cno, out_flow2_cna, out_flow2_cnn;
        rivers_out.update(rivers_out_s);

        deni_status deni_sta(deni_rn0, deni_knb1, deni_Tnc, deni_theta,
                             deni_cnoxc, deni_cnoxo);
        deni_proc.update(deni_sta);

        nitri_status ni_sta(ni_ra0, ni_kab1, ni_foxmin, ni_c_oxc, ni_c_oxo,
                            ni_theta_a, ni_T_c);
        ni_proc.update(ni_sta);

        // 定义更新后的粒子状态,开始状态预测更新dt时间步
        Eigen::VectorXd predict_status(status.size());
        Eigen::VectorXd in  = rivers_in.get_status();
        Eigen::VectorXd out = rivers_out.get_status();
        double ro           = 0;
        double ra           = ni_proc.rate(res_cdo, res_T, res_cna);
        double rn           = deni_proc.rate(res_cdo, res_T, res_cnn);
        reservoir.predict(dt, in(0), out(0), in(1), out(1), in(2), out(2),
                          in(3), out(3), ro, ra, rn);
        res_status next_res_status = reservoir.get_status();
        predict_status(0)          = in_flow1;
        predict_status(1)          = in_flow2;
        predict_status(2)          = out_flow1;
        predict_status(3)          = out_flow2;
        predict_status(4)          = next_res_status.wl;
        return predict_status;
    };
};

class ModelIO : public FilterIO {
  public:
    ModelIO(const std::string &filename_in, const std::string &filename_out)
        : FilterIO(filename_in, filename_out) {}
    ~ModelIO() {}
    void read_one(ExcelIO &file, Eigen::VectorXd &z, Eigen::MatrixXd &R,
                  const int &index) {
        Eigen::VectorXd in_value  = file.read_row("Rivers_in", 2 + index, 2);
        Eigen::VectorXd out_value = file.read_row("Rivers_out", 2 + index, 2);
        Eigen::VectorXd res_value = file.read_row("Reservoir", 2 + index, 2);
        Eigen::VectorXd value(5);
        value << in_value(0), in_value(1), out_value(0), out_value(1),
            res_value(0);

        Eigen::MatrixXd r_value;
        r_value = file.read_block("R", 1, 1, 5, 5);
        z       = value;
        R       = r_value;
    }
};

void run(const std::string &filename_in, const std::string &filename_out) {
    ModelIO modelio(filename_in, filename_out);
    std::unordered_map<std::string, double> params = modelio.get_params();
    EnsembleKalmanFilter<Model> enkal(params["size"]);
    int dims = params["status_dims"];
    Eigen::VectorXd X = modelio.get_init_X("Init_X", 2, 1, dims);
    Eigen::MatrixXd P = modelio.get_init_P("Init_P", 2, 2, dims);
    Eigen::MatrixXd H = modelio.get_H("H", 2, 2, dims, dims);
    Eigen::VectorXd Q = modelio.get_Q("Q", 2, 1, dims);
    enkal.init(X, P, H, Q);
    enkal.batch_assimilation(&modelio, 1);
}

int main(int argc, char *argv[]) {
    auto options             = tools::get_args(argc, argv);
    std::string filename_in  = "Miyun_Model.xlsx";
    std::string filename_out = "Miyun_Model_out.xlsx";
    for (const auto &pair : options) {
        if (pair.first == "-fi") {
            filename_in = pair.second;
        } else if (pair.first == "-fo") {
            filename_out = pair.second;
        } else if (pair.first == "-h") {
            std::cout << "Hello" << std::endl;
        } else {
            std::cout << "Invalid arguments" << std::endl;
        }
    }
    run(filename_in, filename_out);
    return 0;
}
