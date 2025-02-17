/*
 * @Description: 1DReservoirModel.cc
 * @version: v0.2
 * @Author: xfwahss
 * @Date: 2024-07-11 13:37:59
 * @LastEditors: xfwahss
 * @LastEditTime: 2024-07-12 12:27:00
 */
#include <ExcelIO.h>
#include <ReservoirSystem.h>
#include <args.hxx>
#include <logger.h>
#include <optional>

/* @brief 将Excle中存储的[flow, c_tn, cna, cnn] * nums转化为
 *  [flow, c_rpon, clpon, c_don, c_na, cnn] * nums
 *  @param v, 读取的河流状态向量
 * @param frac_nrpon 难降解颗粒有机氮的占比
 * @param frac_nlpon 易降解颗粒有机氮的占比
 * @param nums 河流数目
 */
Eigen::VectorXd read_river_bnd_to_calc_format(const Eigen::VectorXd &v,
                                              const double &frac_nrpon,
                                              const double &frac_nlpon,
                                              const int &nums) {
    Eigen::VectorXd formated_v(River::status_nums * nums);
    for (int i = 0; i < nums; ++i) {
        int read_offset   = 4 * i;
        int format_offset = River::status_nums * i;
        if (v(1 + read_offset) != -999) {
            // 有数据才处理
            double c_no =
                v(1 + read_offset) - v(2 + read_offset) - v(3 + read_offset);
            formated_v(0 + format_offset) = v(0 + read_offset);
            formated_v(1 + format_offset) = frac_nrpon * c_no;
            formated_v(2 + format_offset) = frac_nlpon * c_no;
            formated_v(3 + format_offset) =
                (1 - frac_nlpon - frac_nrpon) * c_no;
            formated_v(4 + format_offset) = v(2 + read_offset);
            formated_v(5 + format_offset) = v(3 + read_offset);
        }
    }
    return formated_v;
}

// 主要是保证空缺值填充为之前的值,前向填充
Eigen::VectorXd fill_forward(Eigen::VectorXd &read_v,
                             const Eigen::VectorXd &pre_v) {
    int nums = read_v.size();
    for (int i = 0; i < nums; i++) {
        if (read_v(i) == -999) {
            read_v(i) = pre_v(i);
        }
    }
    return read_v;
}

void simulate(
    const std::string &filename_in, const std::string &filename_out,
    std::function<double(const double &)> wl_to_volumn,
    std::function<double(const double &, const double &h0)> wl_to_depth,
    std::function<double(const double &)> volumn_to_wl) {
    ExcelIO file_in(filename_in, "r");
    ExcelIO file_out(filename_out, "w");

    auto params      = file_in.read_config("Config", 1, 3, 1);
    int init_step    = DataVariant::stoi(params["init_step"]);
    int steps        = DataVariant::stoi(params["steps"]);
    double dt        = DataVariant::stod(params["dt"]);
    int in_nums      = DataVariant::stoi(params["rivers_in_nums"]);
    int out_nums     = DataVariant::stoi(params["rivers_out_nums"]);
    std::string mode = params["mode"];

    ReservoirSystem::init_system(in_nums, out_nums);

    double h0 = file_in.read_row(params["other_para_sheet"], 2, 2, 4)(2);
    std::function<double(const double &)> wl_to_depth_set =
        [&wl_to_depth, &h0](const double &wl) -> double {
        return wl_to_depth(wl, h0);
    };
    ReservoirSystem::set_wl_volumn_function(wl_to_volumn, wl_to_depth_set,
                                            volumn_to_wl);

    if (mode == std::string("DEBUG")) {
        Logger::get("0DReservoirModel")->set_level(spdlog::level::debug);
    } else if (mode == std::string("INFO")) {
        Logger::get("0DReservoirModel")->set_level(spdlog::level::info);
    }
    // 这里需要读取一个初始状态
    Eigen::VectorXd status = file_in.read_column(params["init_status"], 3, 2,
                                                 WaterColumn::status_nums);

    Eigen::VectorXd rivers_in;
    Eigen::VectorXd rivers_in_read_pre;
    Eigen::VectorXd rivers_out;
    Eigen::VectorXd rivers_out_read_pre;
    Eigen::VectorXd ammon;
    Eigen::VectorXd nitri;
    Eigen::VectorXd denitri;
    Eigen::VectorXd sediment;
    Eigen::VectorXd settling;
    Eigen::VectorXd other;
    for (int i = 0; i < steps; ++i) {
        Logger::get("0DReservoirModel")->info("Index: {}", i + 1);

        other = file_in.read_row(params["other_para_sheet"], i + init_step + 1,
                                 2, 4);
        double frac_nrpon              = other(0);
        double frac_nlpon              = other(1);

        Eigen::VectorXd rivers_in_read = file_in.read_row(
            params["rivers_in_sheet"], i + init_step + 1, 2, in_nums * 4 + 1);
        Eigen::VectorXd rivers_in_filled =
            fill_forward(rivers_in_read, rivers_in_read_pre);
        rivers_in_read_pre = rivers_in_filled;
        rivers_in = read_river_bnd_to_calc_format(rivers_in_filled, frac_nrpon,
                                                  frac_nlpon, in_nums);
        Logger::log_vectorxd("Rivers_in:{}", rivers_in, "0DReservoirModel");

        Eigen::VectorXd rivers_out_read = file_in.read_row(
            params["rivers_out_sheet"], i + init_step + 1, 2, out_nums * 4 + 1);
        Eigen::VectorXd rivers_out_filled =
            fill_forward(rivers_out_read, rivers_out_read_pre);
        rivers_out_read_pre = rivers_out_filled;
        rivers_out          = read_river_bnd_to_calc_format(
            rivers_out_filled, frac_nrpon, frac_nlpon, out_nums);
        Logger::log_vectorxd("Rivers_out:{}", rivers_out, "0DReservoirModel");

        ammon = file_in.read_row(params["amm_para_sheet"], i + init_step + 1, 2,
                                 Ammonification::param_nums + 1);
        Logger::log_vectorxd("ammon:{}", ammon, "0DReservoirModel");

        nitri = file_in.read_row(params["nitri_para_sheet"], i + init_step + 1,
                                 2, Nitrification::param_nums + 1);
        Logger::log_vectorxd("nitri:{}", nitri, "0DReservoirModel");

        denitri = file_in.read_row(params["deni_para_sheet"], i + init_step + 1,
                                   2, Denitrification::param_nums + 1);
        Logger::log_vectorxd("denitri:{}", denitri, "0DReservoirModel");
        sediment =
            file_in.read_row(params["sedi_para_sheet"], i + init_step + 1, 2,
                             Sediment::param_nums + 1);
        Logger::log_vectorxd("sediment:{}", sediment, "0DReservoirModel");
        settling =
            file_in.read_row(params["sett_para_sheet"], i + init_step + 1, 2,
                             ParticleSettle::param_nums + 1);
        Logger::log_vectorxd("settling:{}", settling, "0DReservoirModel");

        Eigen::VectorXd bnd_force(rivers_in.size() + rivers_out.size());
        bnd_force << rivers_in, rivers_out;

        Eigen::VectorXd vector_param(ammon.size() + nitri.size() +
                                     denitri.size() + sediment.size() +
                                     settling.size());
        vector_param << ammon, nitri, denitri, sediment, settling;

        status = ReservoirSystem::predict(status, bnd_force, vector_param,
                                          dt * 86400);

        // 水库的T和DO直接读取
        Eigen::VectorXd do_T_read =
            file_in.read_row(params["res_avg"], i + init_step + 1, 2, 8);
        if (do_T_read(4) != -999) {
            status(7) = do_T_read(4);
        }
        if (do_T_read(5) != -999) {
            status(6) = do_T_read(5);
        }
        file_out.write_row(status, "Simulation", i + 2, 1);
        Logger::log_vectorxd("status:{}", status, "0DReservoirModel");
    }
}
std::string copyfile_and_write_params(
    const std::string &filename, const std::string &sheetname,
    const std::optional<std::string> &id, const std::optional<double> &k_rpon,
    const std::optional<double> &theta_rpon,
    const std::optional<double> &k_lpon,
    const std::optional<double> &theta_lpon, const std::optional<double> &k_don,
    const std::optional<double> &theta_don, const std::optional<double> &rnit0,
    const std::optional<double> &knit20, const std::optional<double> &foxmin,
    const std::optional<double> &c_oxc_nit,
    const std::optional<double> &c_oxo_nit,
    const std::optional<double> &theta_nit,
    const std::optional<double> &T_c_nit, const std::optional<double> &alpha,
    const std::optional<double> &rdeni0, const std::optional<double> &kdeni20,
    const std::optional<double> &Tc_deni,
    const std::optional<double> &theta_deni,
    const std::optional<double> &c_oxc_deni,
    const std::optional<double> &c_oxo_deni, const std::optional<double> &beta,
    const std::optional<double> &b_ndo_flux,
    const std::optional<double> &b_amm_flux,
    const std::optional<double> &b_nit_flux, const std::optional<double> &s_nrp,
    const std::optional<double> &s_nlp, const std::optional<double> &alpha_rpon,
    const std::optional<double> &alpha_lpon, const std::optional<double> &h0) {
    std::filesystem::path file_origin = filename;
    std::filesystem::path copyed_filepath =
        file_origin.parent_path().parent_path() / "temp" /
        (*id + file_origin.filename().string());
    FileIO::copyFile(file_origin, copyed_filepath);
    ExcelIO input(copyed_filepath.string(), "wa");
    int row_i[] = {2,  3,  4,  5,  6,  7,  10, 11, 12, 13, 14, 15, 16, 17, 20,
                   21, 22, 23, 24, 25, 26, 29, 30, 31, 34, 35, 38, 39, 40};
    std::string names[]         = {"Ammonification",
                                   "Ammonification",
                                   "Ammonification",
                                   "Ammonification",
                                   "Ammonification",
                                   "Ammonification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Nitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Denitrification",
                                   "Sediment",
                                   "Sediment",
                                   "Sediment",
                                   "ParticleSettling",
                                   "ParticleSettling",
                                   "Other",
                                   "Other",
                                   "Other"};
    int write_column_i[]        = {2, 3, 4, 5, 6, 7, 2, 3, 4, 5, 6, 7, 8, 9, 2,
                                   3, 4, 5, 6, 7, 8, 2, 3, 4, 2, 3, 2, 3, 4};
    std::optional<double> arr[] = {
        k_rpon,     theta_rpon, k_lpon,     theta_lpon, k_don,      theta_don,
        rnit0,      knit20,     foxmin,     c_oxc_nit,  c_oxo_nit,  theta_nit,
        T_c_nit,    alpha,      rdeni0,     kdeni20,    Tc_deni,    theta_deni,
        c_oxc_deni, c_oxo_deni, beta,       b_ndo_flux, b_amm_flux, b_nit_flux,
        s_nrp,      s_nlp,      alpha_rpon, alpha_lpon, h0};
    int i = 0;
    for (std::optional<double> value : arr) {
        logger::get("0DReservoirModel")->info("Write param: {}", i + 1);
        if (value.has_value()) {
            input.write_cell(*value, sheetname, row_i[i], 3);
            for (int j = 2; j < 369; j++) {
                input.write_cell(*value, names[i], j, write_column_i[i]);
            }
        }
        i++;
    }
    return copyed_filepath.string();
}

template <size_t SIZE>
void parse(int argc, char *argv[], std::string &filename_in,
           std::string &filename_out, std::optional<double> (&v)[SIZE],
           std::string &id) {
    args::ArgumentParser parser("Model a 0D reservoir system",
                                "End of options");
    args::HelpFlag help(parser, "help", "Help menu", {'h', "help"});

    args::ValueFlag<std::string> filein(
        parser, "path", "the filepath of file input", {'i', "input"});
    args::ValueFlag<std::string> fileout(
        parser, "path", "the filepath of file output", {'o', "output"});

    args::ValueFlag<std::string> idin(
        parser, "idin", "add prefix when copy the file input", {"id"});
    args::ValueFlag<double> k_rpon(
        parser, "k_rpon",
        "1 order reaction rate of refactory particle organic nitrogen",
        {"k_rpon"});
    args::ValueFlag<double> theta_rpon(parser, "theta_rpon", "theta_rpon",
                                       {"theta_rpon"});

    args::ValueFlag<double> k_lpon(
        parser, "k_lpon",
        "1 order reaction rate of labile particle organic nitrogen",
        {"k_lpon"});
    args::ValueFlag<double> theta_lpon(parser, "theta_lpon", "theta_lpon",
                                       {"theta_lpon"});

    args::ValueFlag<double> k_don(
        parser, "k_don", "1 order reaction rate of dissolved organic nitrogen",
        {"k_don"});
    args::ValueFlag<double> theta_don(parser, "theta_don", "theta_don",
                                      {"theta_don"});

    args::ValueFlag<double> rnit0(
        parser, "rnit0", "0 order reaction rate of nitrification", {"rnit0"});
    args::ValueFlag<double> knit20(
        parser, "knit20", "1 order reaction rate constant of nitrification",
        {"knit20"});
    args::ValueFlag<double> foxmin(
        parser, "foxmin", "limit of oxygen of nitrification", {"foxmin"});
    args::ValueFlag<double> c_oxc_nit(
        parser, "c_oxc_nit", "critical oxygen of nitrification", {"c_oxc_nit"});
    args::ValueFlag<double> c_oxo_nit(
        parser, "c_oxo_nit", "optimal oxygen of nitrification", {"c_oxo_nit"});
    args::ValueFlag<double> theta_nit(
        parser, "theta_nit", "temperatural coefficient of nitrification",
        {"theta_nit"});
    args::ValueFlag<double> T_c_nit(parser, "T_c_nit",
                                    "critical temperature of nitrification",
                                    {"T_c_nit"});
    args::ValueFlag<double> alpha(parser, "alpha", "curvlinear rate",
                                  {"alpha"});

    args::ValueFlag<double> rdeni0(parser, "rdeni0",
                                   "0 order reaction rate of denitrification",
                                   {"rdeni0"});
    args::ValueFlag<double> kdeni20(
        parser, "kdeni20", "1 order reaction rate constant of denitrification",
        {"kdeni20"});
    args::ValueFlag<double> Tc_deni(parser, "Tc_deni",
                                    "critical temperature of denitrification",
                                    {"Tc_deni"});
    args::ValueFlag<double> theta_deni(
        parser, "theta_deni", "temperatural coefficient of denitrification",
        {"theta_deni"});
    args::ValueFlag<double> c_oxc_deni(parser, "c_oxc_deni",
                                       "critical oxygen of denitrification",
                                       {"c_oxc_deni"});
    args::ValueFlag<double> c_oxo_deni(parser, "c_oxo_deni",
                                       "optimal oxygen of denitrification",
                                       {"c_oxo_deni"});
    args::ValueFlag<double> beta(parser, "beta", "curvature rate", {"beta"});

    args::ValueFlag<double> b_ndo_flux(parser, "b_ndo_flux", "b_ndo_flux",
                                       {"b_ndo_flux"});
    args::ValueFlag<double> b_amm_flux(parser, "b_amm_flux", "b_amm_flux",
                                       {"b_amm_flux"});
    args::ValueFlag<double> b_nit_flux(parser, "b_nit_flux", "b_nit_flux",
                                       {"b_nit_flux"});

    args::ValueFlag<double> s_nrp(parser, "s_nrp", "s_nrp", {"s_nrp"});
    args::ValueFlag<double> s_nlp(parser, "s_nlp", "s_nlp", {"s_nlp"});

    args::ValueFlag<double> alpha_rpon(parser, "alpha_rpon", "alpha_rpon",
                                       {"alpha_rpon"});
    args::ValueFlag<double> alpha_lpon(parser, "alpha_lpon", "alpha_lpon",
                                       {"alpha_lpon"});
    args::ValueFlag<double> h0(parser, "h0", "h0", {"h0"});
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
    if (filein) {
        filename_in = args::get(filein);
    }
    if (fileout) {
        filename_out = args::get(fileout);
    }
    if (idin) {
        id = args::get(idin);
    }
    if (k_rpon) {
        v[0] = args::get(k_rpon);
    }
    if (theta_rpon) {
        v[1] = args::get(theta_rpon);
    }
    if (k_lpon) {
        v[2] = args::get(k_lpon);
    }
    if (theta_lpon) {
        v[3] = args::get(theta_lpon);
    }
    if (k_don) {
        v[4] = args::get(k_don);
    }
    if (theta_don) {
        v[5] = args::get(theta_don);
    }

    if (rnit0) {
        v[6] = args::get(rnit0);
    }
    if (knit20) {
        v[7] = args::get(knit20);
    }
    if (foxmin) {
        v[8] = args::get(foxmin);
    }
    if (c_oxc_nit) {
        v[9] = args::get(c_oxc_nit);
    }
    if (c_oxo_nit) {
        v[10] = args::get(c_oxo_nit);
    }
    if (theta_nit) {
        v[11] = args::get(theta_nit);
    }
    if (T_c_nit) {
        v[12] = args::get(T_c_nit);
    }
    if (alpha) {
        v[13] = args::get(alpha);
    }

    if (rdeni0) {
        v[14] = args::get(rdeni0);
    }
    if (kdeni20) {
        v[15] = args::get(kdeni20);
    }
    if (Tc_deni) {
        v[16] = args::get(Tc_deni);
    }
    if (theta_deni) {
        v[17] = args::get(theta_deni);
    }
    if (c_oxc_deni) {
        v[18] = args::get(c_oxc_deni);
    }
    if (c_oxo_deni) {
        v[19] = args::get(c_oxo_deni);
    }
    if (beta) {
        v[20] = args::get(beta);
    }
    if (b_ndo_flux) {
        v[21] = args::get(b_ndo_flux);
    }
    if (b_amm_flux) {
        v[22] = args::get(b_amm_flux);
    }
    if (b_nit_flux) {
        v[23] = args::get(b_nit_flux);
    }
    if (s_nrp) {
        v[24] = args::get(s_nrp);
    }
    if (s_nlp) {
        v[25] = args::get(s_nlp);
    }
    if (alpha_rpon) {
        v[26] = args::get(alpha_rpon);
    }
    if (alpha_lpon) {
        v[27] = args::get(alpha_lpon);
    }
    if (h0) {
        v[28] = args::get(h0);
    }
}

double wl_to_volume(const double &wl) {
    return 0.022846 * wl * wl - 5.341818 * wl + 314.495276;
}

double wl_to_depth(const double &wl, const double &h0) { return wl - h0; }

double volume_to_wl(const double &volume) {
    double a = 0.022846;
    double b = -5.341818;
    double c = 314.495276 - volume;
    double x = (-b + std::sqrt(b * b - 4 * a * c)) / a / 2;
    return x;
}

int main(int argc, char *argv[]) {
    std::string filename_in, filename_out, id;
    std::optional<double> value[29];
    parse(argc, argv, filename_in, filename_out, value, id);
    std::string copyed_file = copyfile_and_write_params(
        filename_in, "Parameters", id, value[0], value[1], value[2], value[3],
        value[4], value[5], value[6], value[7], value[8], value[9], value[10],
        value[11], value[12], value[13], value[14], value[15], value[16],
        value[17], value[18], value[19], value[20], value[21], value[22],
        value[23], value[24], value[25], value[26], value[27], value[28]);
    simulate(copyed_file, filename_out, wl_to_volume, wl_to_depth,
             volume_to_wl);
    return 0;
}