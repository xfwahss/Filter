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
        double in_flow1_cno = status(5);
        double in_flow1_cna = status(6);
        double in_flow1_cnn = status(7);

        double in_flow2     = status(1);
        double in_flow2_cno = status(8);
        double in_flow2_cna = status(9);
        double in_flow2_cnn = status(10);

        double out_flow1     = status(2);
        double out_flow1_cno = status(11);
        double out_flow1_cna = status(12);
        double out_flow1_cnn = status(13);

        double out_flow2     = status(3);
        double out_flow2_cno = status(14);
        double out_flow2_cna = status(15);
        double out_flow2_cnn = status(16);

        double wl      = status(4);
        double res_cno = status(17);
        double res_cna = status(18);
        double res_cnn = status(19);
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
        // ra = 0;
        // rn = 0;
        reservoir.predict(dt, in(0), out(0), in(1), out(1), in(2), out(2),
                          in(3), out(3), ro, ra, rn);
        res_status next_res_status = reservoir.get_status();
        predict_status(0)          = in_flow1;
        predict_status(1)          = in_flow2;
        predict_status(2)          = out_flow1;
        predict_status(3)          = out_flow2;
        predict_status(4)          = next_res_status.wl;
        predict_status(5) = in_flow1_cno;
        predict_status(6) = in_flow1_cna;
        predict_status(7) = in_flow1_cnn;
        predict_status(8) = in_flow2_cno;
        predict_status(9) = in_flow2_cna;
        predict_status(10) = in_flow2_cnn;
        predict_status(11) = out_flow1_cno;
        predict_status(12) = out_flow1_cna;
        predict_status(13) = out_flow1_cnn;
        predict_status(14) = out_flow2_cno;
        predict_status(15) = out_flow2_cna;
        predict_status(16) = out_flow2_cnn;
        predict_status(17) = next_res_status.c_no;
        predict_status(18) = next_res_status.c_na;
        predict_status(19) = next_res_status.c_nn;
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
        using tools::exclude_mean;
        using tools::handle_miss;

        //*****************************初始化*******************************
        int obs_dims = get_params("Params", 1, 2, 1)["obs_dims"];
        Eigen::VectorXd z_value(obs_dims);
        Eigen::MatrixXd r_value(obs_dims, obs_dims);

        //*****************************数据读取*****************************
        /* in_value的数据格式:
         * 0: baihe flow; 1: chaohe flow
         * 2: baihe NHN; 3: baihe NON 4: baihe TN
         * 5: chaohe NHN 6: chaohe NON 7: chaohe TN
         */
        Eigen::VectorXd in_value = file.read_row("Rivers_in", 2 + index, 2);

        /* out_value的数据格式：
         * 0: baihe_dam flow; 1: chaohe_dam flow
         * 2: baihe_dam NHN; 3: baihe_dam NON; 4: baihe_dam TN
         * 5: chaohe_dam NHN; 6: chaohe_dam NON; 7: chaohe_dam TN
         */
        Eigen::VectorXd out_value = file.read_row("Rivers_out", 2 + index, 2);

        /*  res_value的数据格式:
         * 0: water level
         * 1: kuxi_T 2:Kuxi_DO 3:kuxi_NHN 4:kuxi_NON 5:kuxi_TN
         * 6: Taoli_T 7:Taoli_DO 8:Taoli_NHN 9:Taoli_NON 10:Taoli_TN
         * 11: Neihu_T 12:Neihu_DO 13:Neihu_NHN 14:Neihu_NON 15:Neihu_TN
         * 16: Henghe_T 17:Henghe_DO 18:Henghe_NHN 19:Henghe_NON 20:Henghe_TN
         * 21: Kudong_T 22:Kudong_DO 23:Kudong_NHN 24:Kudong_NON 25:Kudong_TN
         * 26: Jingou_T 27:Jingou_DO 28:Jingou_NHN 29:Jingou_NON 30:Jingou_TN
         */
        Eigen::VectorXd res_value = file.read_row("Reservoir", 2 + index, 2);

        //***************************数据处理**************************
        // 水文数据
        z_value(0) = in_value(0);
        z_value(1) = in_value(1);
        z_value(2) = out_value(0);
        z_value(3) = out_value(1);
        z_value(4) = res_value(0);

        // 水质数据
        double baihe_cno = handle_miss((in_value(4))) -
                           handle_miss(in_value(3)) - handle_miss(in_value(2));
        double chaohe_cno = handle_miss(in_value(7)) -
                            handle_miss(in_value(6)) - handle_miss(in_value(5));
        z_value(5)  = baihe_cno;
        z_value(6)  = handle_miss(in_value(2));
        z_value(7)  = handle_miss(in_value(3));
        z_value(8)  = chaohe_cno;
        z_value(9)  = handle_miss(in_value(5));
        z_value(10) = handle_miss(in_value(6));

        double baihedam_cno = handle_miss(out_value(4)) -
                              handle_miss(out_value(3)) -
                              handle_miss(out_value(2));
        double chaohedam_cno = handle_miss(out_value(7)) -
                               handle_miss(out_value(6)) -
                               handle_miss(out_value(5));
        z_value(11) = baihedam_cno;
        z_value(12) = handle_miss(out_value(2));
        z_value(13) = handle_miss(out_value(3));
        z_value(14) = chaohedam_cno;
        z_value(15) = handle_miss(out_value(5));
        z_value(16) = handle_miss(out_value(6));

        double res_T, res_do, res_cna, res_cnn, res_cno;
        res_T   = exclude_mean({res_value(1), res_value(6), res_value(11),
                                res_value(16), res_value(21), res_value(26)});
        res_do  = exclude_mean({res_value(2), res_value(7), res_value(12),
                                res_value(17), res_value(22), res_value(27)});
        res_cna = exclude_mean({res_value(3), res_value(8), res_value(13),
                                res_value(18), res_value(23), res_value(28)});
        res_cnn = exclude_mean({res_value(4), res_value(9), res_value(14),
                                res_value(19), res_value(24), res_value(29)});
        res_cno = exclude_mean({res_value(5), res_value(10), res_value(15),
                                res_value(20), res_value(25), res_value(30)}) -
                  res_cna - res_cnn;
        z_value(17) = res_cno;
        z_value(18) = res_cna;
        z_value(19) = res_cnn;

        r_value     = file.read_block("R", 2, 2, obs_dims, obs_dims);

        // 值传递
        z = z_value;
        R = r_value;
    }
};

void run(const std::string &filename_in, const std::string &filename_out) {
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
