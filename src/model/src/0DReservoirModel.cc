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
void write_process_params(
    const std::string &filename, const std::string &sheetname,
    const std::optional<double> &ro0, const std::optional<double> &ko1,
    const std::optional<double> &ra0, const std::optional<double> &kab1,
    const std::optional<double> &foxmin, const std::optional<double> &c_oxc,
    const std::optional<double> &c_oxo, const std::optional<double> &theta_a,
    const std::optional<double> &T_c, const std::optional<double> &rn0,
    const std::optional<double> &knb1, const std::optional<double> &Tnc,
    const std::optional<double> &theta_n, const std::optional<double> &c_noxc,
    const std::optional<double> &c_noxo) {
    std::filesystem::path file_origin = filename;
    FileIO::copyFile(file_origin, file_origin.parent_path().parent_path() /
                                      "temp" / file_origin.filename());
    ExcelIO input(filename, "wa");
    int row_i[]         = {2, 3, 2, 3, 4, 5, 6, 7, 8, 2, 3, 4, 5, 6, 7};
    int column_i[]      = {2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6};
    std::string names[] = {
        "Ammonification",  "Ammonification",  "Nitrification",
        "Nitrification",   "Nitrification",   "Nitrification",
        "Nitrification",   "Nitrification",   "Nitrification",
        "Denitrification", "Denitrification", "Denitrification",
        "Denitrification", "Denitrification", "Denitrification"};
    int write_column_i[]        = {2, 3, 2, 3, 4, 5, 6, 7, 8, 2, 3, 4, 5, 6, 7};
    std::optional<double> arr[] = {ro0,   ko1,   ra0,     kab1,   foxmin,
                                   c_oxc, c_oxo, theta_a, T_c,    rn0,
                                   knb1,  Tnc,   theta_n, c_noxc, c_noxo};
    int i                       = 0;
    for (std::optional<double> value : arr) {
        if (value.has_value()) {
            input.write_cell(*value, sheetname, row_i[i], column_i[i]);
            for (int j = 2; j < 2195; j++) {
                input.write_cell(*value, names[i], j, write_column_i[i]);
            }
        }
        i++;
    }
}

template <size_t SIZE>
void parse(int argc, char *argv[], std::string &filename_in,
           std::string &filename_out, std::optional<double> (&v)[SIZE]) {
    args::ArgumentParser parser("Model a 0D reservoir system",
                                "End of options");
    args::HelpFlag help(parser, "help", "Help menu", {'h', "help"});

    args::ValueFlag<std::string> filein(
        parser, "path", "the filepath of file input", {'i', "input"});
    args::ValueFlag<std::string> fileout(
        parser, "path", "the filepath of file output", {'o', "output"});

    args::ValueFlag<double> ro0(
        parser, "ro0", "0 order reaction rate of ammonification", {"ro0"});
    args::ValueFlag<double> ko1(
        parser, "ra0", "1 order reaction rate constant of ammonification",
        {"ko1"});

    args::ValueFlag<double> ra0(
        parser, "ra0", "0 order reaction rate of nitrification", {"ra0"});
    args::ValueFlag<double> kab1(
        parser, "kab1", "1 order reaction rate constant of nitrification",
        {"kab1"});
    args::ValueFlag<double> foxmin(
        parser, "foxmin", "limit of oxygen of nitrification", {"foxmin"});
    args::ValueFlag<double> c_oxc(
        parser, "c_oxc", "critical oxygen of nitrification", {"c_oxc"});
    args::ValueFlag<double> c_oxo(parser, "c_oxo",
                                  "optimal oxygen of nitrification", {"c_oxo"});
    args::ValueFlag<double> theta_a(parser, "theta_a",
                                    "temperatural coefficient of nitrification",
                                    {"theta_a"});
    args::ValueFlag<double> T_c(
        parser, "T_c", "critical temperature of nitrification", {"T_c"});

    args::ValueFlag<double> rn0(
        parser, "rn0", "0 order reaction rate of denitrification", {"rn0"});
    args::ValueFlag<double> knb1(
        parser, "knb1", "1 order reaction rate constant of denitrification",
        {"knb1"});
    args::ValueFlag<double> Tnc(
        parser, "Tnc", "critical temperature of denitrification", {"Tnc"});
    args::ValueFlag<double> theta_n(
        parser, "theta_n", "temperatural coefficient of denitrification",
        {"theta_n"});
    args::ValueFlag<double> c_noxc(
        parser, "c_noxc", "critical oxygen of denitrification", {"c_noxc"});
    args::ValueFlag<double> c_noxo(
        parser, "c_noxo", "optimal oxygen of denitrification", {"c_noxo"});
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
    if (ro0) {
        v[0] = args::get(ro0);
    }
    if (ko1) {
        v[1] = args::get(ko1);
    }

    if (ra0) {
        v[2] = args::get(ra0);
    }
    if (kab1) {
        v[3] = args::get(kab1);
    }
    if (foxmin) {
        v[4] = args::get(foxmin);
    }
    if (c_oxc) {
        v[5] = args::get(c_oxc);
    }
    if (c_oxo) {
        v[6] = args::get(c_oxo);
    }
    if (theta_a) {
        v[7] = args::get(theta_a);
    }
    if (T_c) {
        v[8] = args::get(T_c);
    }
    if (rn0) {
        v[9] = args::get(rn0);
    }
    if (knb1) {
        v[10] = args::get(knb1);
    }
    if (Tnc) {
        v[11] = args::get(Tnc);
    }
    if (theta_n) {
        v[12] = args::get(theta_n);
    }
    if (c_noxc) {
        v[13] = args::get(c_noxc);
    }
    if (c_noxo) {
        v[14] = args::get(c_noxo);
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
    std::filesystem::path filename_in, filename_out;
    filename_in                     = "test/data/0DReservoirModel_origin.xlsx";
    filename_out                    = "test/data/0DReservoirModel_out.xlsx";
    // std::optional<double> value[15];
    // parse(argc, argv, filename_in, filename_out, value);

    // write_process_params(filename_in, "process_lib", value[0], value[1],
    //                      value[2], value[3], value[4], value[5], value[6],
    //                      value[7], value[8], value[9], value[10], value[11],
    //                      value[12], value[13], value[14]);
    // simulate(filename_in.string(), filename_out.string(), wl_to_volume,
    // wl_to_depth,
    //          volume_to_wl);
    std::filesystem::path file_test = "test/data/0DReservoirModel_out.xlsx";
    FileIO::copyFile(file_test, file_test.parent_path().parent_path() / "temp" /
                                    file_test.filename());
    return 0;
}