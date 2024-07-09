#include "../../io/include/ExcelIO.h"
#include "../../reactor/include/Ammonification.h"
#include "../../reactor/include/Denitrification.h"
#include "../../reactor/include/Nitrification.h"
#include "../../reactor/include/Reservoir.h"
#include "../../reactor/include/RiverSystem.h"

class ReservoirIO {
  private:
    ExcelIO file_in;
    ExcelIO file_out;

    int read_index;
    int write_index;

  public:
    int data_nums;
    int input_cols;
    int res_cols;
    int coef_cols;
    int legenth;
    ReservoirIO(const std::string &filename_in,
                const std::string &filename_out);
    ~ReservoirIO() = default;
    Eigen::VectorXd read_one();
    std::unordered_map<std::string, double> read_init();
    void write(const Eigen::VectorXd &value);
};
ReservoirIO::ReservoirIO(const std::string &filename_in,
                         const std::string &filename_out)
    : file_in(filename_in, "r"), file_out(filename_out, "w") {
    std::unordered_map<std::string, double> params =
        file_in.read_dict("Params", 1, 2);
    data_nums   = params["nums"];
    input_cols  = params["input_cols"];
    res_cols    = params["res_cols"];
    coef_cols   = params["coef_cols"];
    legenth     = input_cols + coef_cols + res_cols;
    read_index  = 1;
    write_index = 1;
}
Eigen::VectorXd ReservoirIO::read_one() {
    Eigen::VectorXd values(legenth);
    Eigen::VectorXd input =
        file_in.read_row("Input", read_index + 1, 2, input_cols + 1);
    Eigen::VectorXd res =
        file_in.read_row("Res_Force", read_index + 1, 2, res_cols + 1);
    Eigen::VectorXd coef =
        file_in.read_row("Coefficients", read_index + 1, 2, coef_cols + 1);
    values << input, res, coef;
    ++read_index;
    return values;
}

std::unordered_map<std::string, double> ReservoirIO::read_init() {
    std::unordered_map<std::string, double> params =
        file_in.read_dict("Params", 4, 5, 1);
    return params;
}

void ReservoirIO::write(const Eigen::VectorXd &value) {
    if (write_index == 1) {
        std::vector<std::string> names;
        names.push_back("wl");
        names.push_back("c_no");
        names.push_back("c_na");
        names.push_back("c_nn");
        names.push_back("T");
        names.push_back("c_do");
        file_out.write_header(names, "Value", write_index);
        ++write_index;
    };
    file_out.write_row(value, "Value", write_index);
    ++write_index;
}

class ReservoirModel {
  private:
    RiverSystem rivers_in;
    RiverSystem rivers_out;
    Reservoir res;
    Ammonification ammon;
    Nitrification nitri;
    Denitrification deni;

  public:
    ReservoirModel()  = default;
    ~ReservoirModel() = default;
    ReservoirModel(RiverSystem &rivers_in, RiverSystem &rivers_out,
                   Reservoir &res, Ammonification &ammon, Nitrification &nitri,
                   Denitrification &deni);
    Eigen::VectorXd read_force(ReservoirIO &file);
    void update_force(const Eigen::VectorXd &river_in_stat,
                      const Eigen::VectorXd &river_out_stat,
                      ammon_status ammon_stat, nitri_status ni_stat,
                      deni_status deni_stat, const double &res_T,
                      const double &res_do);
    void predict(const double &dt);
    void simulate(const double &dt, ReservoirIO &file);
};

ReservoirModel::ReservoirModel(RiverSystem &rivers_in, RiverSystem &rivers_out,
                               Reservoir &res, Ammonification &ammon,
                               Nitrification &nitri, Denitrification &deni)
    : rivers_in(rivers_in), rivers_out(rivers_out), res(res), ammon(ammon),
      nitri(nitri), deni(deni) {}

void ReservoirModel::update_force(const Eigen::VectorXd &river_in_stat,
                                  const Eigen::VectorXd &river_out_stat,
                                  ammon_status ammon_stat, nitri_status ni_stat,
                                  deni_status deni_stat, const double &res_T,
                                  const double &res_do) {
    rivers_in.update(river_in_stat);
    rivers_out.update(river_out_stat);
    ammon.update(ammon_stat);
    nitri.update(ni_stat);
    deni.update(deni_stat);

    res_status update_res(res.get_status().wl, res.get_status().c_no,
                          res.get_status().c_na, res.get_status().c_nn, res_T,
                          res_do);
    res.update(update_res);
}

void ReservoirModel::predict(const double &dt) {
    // std::cout << "Before--wl: " << res.get_status().wl
    //           << ", c_no: " << res.get_status().c_no
    //           << ", c_na: " << res.get_status().c_na
    //           << ", c_nn: " << res.get_status().c_nn << std::endl;
    double flow_in, load_org_in, load_amm_in, load_nit_in;
    flow_in     = rivers_in.get_status()(0);
    load_org_in = rivers_in.get_status()(1);
    load_amm_in = rivers_in.get_status()(2);
    load_nit_in = rivers_in.get_status()(3);

    double flow_out, load_org_out, load_amm_out, load_nit_out;
    flow_out     = rivers_out.get_status()(0);
    load_org_out = rivers_out.get_status()(1);
    load_amm_out = rivers_out.get_status()(2);
    load_nit_out = rivers_out.get_status()(3);

    res_status res_cur = res.get_status();

    double ro = ammon.rate(res_cur.c_no);
    double rn = deni.rate(res_cur.c_do, res_cur.T, res_cur.c_nn);
    double ra = nitri.rate(res_cur.c_do, res_cur.T, res_cur.c_na);
    // std::cout << ro << "    "<< ra << "    " << rn << std::endl;

    res.predict(dt, flow_in, flow_out, load_org_in, load_org_out, load_amm_in,
                load_amm_out, load_nit_in, load_nit_out, ro, ra, rn);
    std::cout << "wl: " << res.get_status().wl
              << ", c_no: " << res.get_status().c_no
              << ", c_na: " << res.get_status().c_na
              << ", c_nn: " << res.get_status().c_nn << std::endl;
}

void ReservoirModel::simulate(const double &dt, ReservoirIO &file) {
    int i = 0;
    while (i < file.data_nums) {
        Eigen::VectorXd value     = file.read_one();
        Eigen::VectorXd river_in  = value.segment(0, 8);
        Eigen::VectorXd river_out = value.segment(8, 8);
        double res_T              = value(16);
        double res_do             = value(17);
        ammon_status amm_sta;
        amm_sta = Eigen::VectorXd(value.segment(18, 2));
        nitri_status ni_sta;
        ni_sta = Eigen::VectorXd(value.segment(20, 7));
        deni_status deni_sta;
        deni_sta = value.segment(27, 6);
        update_force(river_in, river_out, amm_sta, ni_sta, deni_sta, res_T,
                     res_do);
        predict(dt);
        Eigen::VectorXd res_status = res.get_status();
        file.write(res_status);
        ++i;
    }
}

int main() {
    River baihe, chaohe, baihe_out, chaohe_out;
    RiverSystem rivers_in, rivers_out;
    rivers_in.add_river("Baihe", baihe);
    rivers_in.add_river("Chaohe", chaohe);
    rivers_out.add_river("Baihe_Dam", baihe_out);
    rivers_out.add_river("Chaohe_Dam", chaohe_out);
    Reservoir res;
    Ammonification ammon;
    Nitrification nitri;
    Denitrification deni;

    ReservoirIO resio("../test/data/Synthetic_Modeling.xlsx",
                      "../test/output/Synthetic_Modeling_Out.xlsx");
    auto params = resio.read_init();
    // 子系统初始化
    Eigen::VectorXd in(8), out(8);
    in << params["Baihe_Flow"], params["Baihe_Cno"], params["Baihe_Cna"],
        params["Baihe_Cnn"], params["Chaohe_Flow"], params["Chaohe_Cno"],
        params["Chaohe_Cna"], params["Chaohe_Cnn"];
    out << params["Baihe_Dam_Flow"], params["Baihe_Dam_Cno"],
        params["Baihe_Dam_Cna"], params["Baihe_Dam_Cnn"],
        params["Chaohe_Dam_Flow"], params["Chaohe_Dam_Cno"],
        params["Chaohe_Dam_Cna"], params["Chaohe_Dam_Cnn"];
    rivers_in.update(in);
    rivers_out.update(out);

    double wl, c_no, c_na, c_nn, T, c_do;
    wl = params["wl"], c_no = params["res_cno"], c_na = params["res_cna"],
    c_nn = params["res_cnn"], T = params["T"], c_do = params["C_do"];
    res.init(wl, c_no, c_na, c_nn, T, c_do);

    ammon.init(params["ro0"], params["ko1"]);

    double ra0, kab1, foximin, c_oxc, c_oxo, theta_a, T_c;
    ra0 = params["ra0"], kab1 = params["kab1"], foximin = params["foximin"],
    c_oxc = params["c_oxc"], c_oxo = params["c_oxo"],
    theta_a = params["theta_a"], T_c = params["T_c"];
    nitri.init(ra0, kab1, foximin, c_oxc, c_oxo, theta_a, T_c);

    double rn0, knb1, Tnc, theta_n, c_noxc, c_noxo;
    rn0 = params["rn0"], knb1 = params["knb1"], Tnc = params["Tnc"],
    theta_n = params["theta_n"], c_noxc = params["c_noxc"],
    c_noxo = params["c_noxo"];
    deni.init(rn0, knb1, Tnc, theta_n, c_noxc, c_noxo);

    ReservoirModel model(rivers_in, rivers_out, res, ammon, nitri, deni);

    model.simulate(1, resio);
    // reader
    return 0;
}