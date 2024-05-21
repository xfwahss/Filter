#include "../../filter/include/EnsembleKalmanFilter.h"
#include "../../reactor/include/Reservoir.h"
#include "../../reactor/include/RiverSystem.h"

class Model : public EnsembleModel {
  public:
    Eigen::VectorXd predict(const double &dt) {
        Reservoir reservoir;
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
        // 更新粒子状态
        Eigen::VectorXd current_status = get_status();
        Eigen::VectorXd predict_status(current_status.size());
        // 更新子模型状态
        res_status res_s(current_status(4), 0, 0, 0, 0, 0);
        Eigen::VectorXd rivers_in_s(8);
        rivers_in_s << current_status(0), 0, 0, 0, current_status(1), 0, 0, 0;
        Eigen::VectorXd rivers_out_s(8);
        rivers_out_s << current_status(2), 0, 0, 0, current_status(3), 0, 0, 0;
        reservoir.update(res_s);
        rivers_in.update(rivers_in_s);
        rivers_out.update(rivers_out_s);

        // 子模型预测dt
        Eigen::VectorXd in  = rivers_in.get_status();
        Eigen::VectorXd out = rivers_out.get_status();
        double ro = 0, ra = 0, rn = 0;
        reservoir.predict(dt, in(0), out(0), in(1), out(1), in(2), out(2),
                          in(3), out(3), ro, ra, rn);
        res_status next_res_status = reservoir.get_status();
        predict_status(0)          = current_status(0);
        predict_status(1)          = current_status(1);
        predict_status(2)          = current_status(2);
        predict_status(3)          = current_status(3);
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
int main() {
    EnsembleKalmanFilter<Model> enkal(2000);
    ModelIO modelio("../test/data/Miyun_Model.xlsx",
                    "../test/data/Miyun_Model_out.xlsx");
    enkal.init(modelio.get_init_X(), modelio.get_init_P(), modelio.get_H(),
               modelio.get_Q());
    enkal.batch_assimilation(&modelio, 1);
    return 0;
}
