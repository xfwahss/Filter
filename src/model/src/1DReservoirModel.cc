/*
 * @Description: 1DReservoirModel.cc
 * @version: v0.1
 * @Author: xfwahss
 * @Date: 2024-07-11 13:37:59
 * @LastEditors: xfwahss
 * @LastEditTime: 2024-07-12 12:27:00
 */
#include <DataStructures.h>
#include <ExcelIO.h>
#include <ReservoirSystem.h>
#include <args.hxx>
#include <logger.h>
#include <optional>

Eigen::VectorXd to_org_amm_nit(Eigen::VectorXd &v) {
    int nums = v.size() / RiverStatus::size;
    for (int i = 0; i < nums; ++i) {
        if (v(1 + i * RiverStatus::size) != -999) {
            v(1 + i * RiverStatus::size) =
                v(1 + i * RiverStatus::size) - v(2 + i * RiverStatus::size) - v(3 + i * RiverStatus::size);
        }
    }
    return v;
}

Eigen::VectorXd extract_flow(Eigen::VectorXd &read_v, const Eigen::VectorXd &pre_v) {
    int nums = read_v.size();
    for (int i = 0; i < nums; i++) {
        if (read_v(i) == -999) {
            read_v(i) = pre_v(i);
        }
    }
    return read_v;
}

void simulate(const std::string &filename_in, const std::string &filename_out) {
    ExcelIO file_in(filename_in, "r");
    ExcelIO file_out(filename_out, "w");

    auto params      = file_in.read_config("config", 1, 2, 1);
    int init_step    = DataVariant::stoi(params["init_step"]);
    int steps        = DataVariant::stoi(params["steps"]);
    double dt        = DataVariant::stod(params["dt"]);
    int in_nums      = DataVariant::stoi(params["rivers_in_nums"]);
    int out_nums     = DataVariant::stoi(params["rivers_out_nums"]);
    std::string mode = params["mode"];

    if (mode == std::string("debug")) {
        logger::get("ReservoirSystem-cc")->set_level(spdlog::level::debug);
    } else if (mode == std::string("info")) {
        spdlog::set_level(spdlog::level::info);
    }

    // 这里需要读取一个初始状态
    Eigen::VectorXd status = file_in.read_column(params["init_status"], 2, 1, ReservoirStatus::size);

    Eigen::VectorXd rivers_in;
    Eigen::VectorXd rivers_out;
    Eigen::VectorXd ammon_k;
    Eigen::VectorXd nitri_k;
    Eigen::VectorXd denitri_k;
    Eigen::VectorXd bnd_force;
    for (int i = 0; i < steps; ++i) {
        logger::get("1DReservoirModel")->info("Index: {}", i + 1);

        Eigen::VectorXd rivers_in_read =
            file_in.read_row(params["rivers_in_name"], i + init_step + 1, 2, in_nums * RiverStatus::size + 1);
        rivers_in_read = to_org_amm_nit(rivers_in_read);
        rivers_in      = extract_flow(rivers_in_read, rivers_in);

        logger::log_vectorxd("rivers_in:{}", rivers_in, "1DReservoirModel");

        Eigen::VectorXd rivers_out_read =
            file_in.read_row(params["rivers_out_name"], i + init_step + 1, 2, out_nums * RiverStatus::size + 1);
        rivers_out_read = to_org_amm_nit(rivers_out_read);
        rivers_out      = extract_flow(rivers_out_read, rivers_out);
        logger::log_vectorxd("rivers_out:{}", rivers_out, "1DReservoirModel");

        ammon_k = file_in.read_row(params["ammonification_para"], i + init_step + 1, 2, AmmonificationStatus::size + 1);
        logger::log_vectorxd("ammon_k:{}", ammon_k, "1DReservoirModel");

        nitri_k = file_in.read_row(params["nitrification_para"], i + init_step + 1, 2, NitrificationStatus::size + 1);
        logger::log_vectorxd("nitri_k:{}", nitri_k, "1DReservoirModel");

        denitri_k = file_in.read_row(params["denitrification_para"], i + init_step + 1, 2, DenificationStatus::size + 1);
        logger::log_vectorxd("denitri_k:{}", denitri_k, "1DReservoirModel");

        Eigen::VectorXd bnd_force(rivers_in.size() + rivers_out.size() + ammon_k.size() + nitri_k.size() +
                                  denitri_k.size());
        bnd_force << rivers_in, rivers_out, ammon_k, nitri_k, denitri_k;

        status                    = ReservoirSystem::predict(status, bnd_force, in_nums, out_nums, dt);

        // 水库的T和DO直接读取
        Eigen::VectorXd do_T_read = file_in.read_row(params["res_avg"], i + init_step + 1, 2, ReservoirStatus::size + 1);
        if (do_T_read(4) != -999) {
            status(4) = do_T_read(4);
        }
        if (do_T_read(5) != -999) {
            status(5) = do_T_read(5);
        }

        file_out.write_row(status, "Simulation", i + 2, 1);
    }
}
void write_process_params(const std::string &filename, const std::string &sheetname, const std::optional<double> &ro0,
                          const std::optional<double> &ko1, const std::optional<double> &ra0,
                          const std::optional<double> &kab1, const std::optional<double> &foxmin,
                          const std::optional<double> &c_oxc, const std::optional<double> &c_oxo,
                          const std::optional<double> &theta_a, const std::optional<double> &T_c,
                          const std::optional<double> &rn0, const std::optional<double> &knb1,
                          const std::optional<double> &Tnc, const std::optional<double> &theta_n,
                          const std::optional<double> &c_noxc, const std::optional<double> &c_noxo) {
    ExcelIO input(filename, "wa");
    int row_i[]                 = {2, 3, 2, 3, 4, 5, 6, 7, 8, 2, 3, 4, 5, 6, 7};
    int column_i[]              = {2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6};
    std::string names[]         = {"Ammonification",  "Ammonification",  "Nitrification",   "Nitrification",
                                   "Nitrification",   "Nitrification",   "Nitrification",   "Nitrification",
                                   "Nitrification",   "Denitrification", "Denitrification", "Denitrification",
                                   "Denitrification", "Denitrification", "Denitrification"};
    int write_column_i[]        = {2, 3, 2, 3, 4, 5, 6, 7, 8, 2, 3, 4, 5, 6, 7};
    std::optional<double> arr[] = {ro0, ko1, ra0,  kab1, foxmin,  c_oxc,  c_oxo, theta_a,
                                   T_c, rn0, knb1, Tnc,  theta_n, c_noxc, c_noxo};
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
void parse(int argc, char *argv[], std::string &filename_in, std::string &filename_out,
           std::optional<double> (&v)[SIZE]) {
    args::ArgumentParser parser("Model a 1D reservoir system", "End of options");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

    args::ValueFlag<std::string> filein(parser, "path", "the filepath of file input", {'i', "input"});
    args::ValueFlag<std::string> fileout(parser, "path", "the filepath of file output", {'o', "output"});

    args::ValueFlag<double> ro0(parser, "ro0", "0 order reaction rate of ammonification", {"ro0"});
    args::ValueFlag<double> ko1(parser, "ra0", "1 order reaction rate constant of ammonification", {"ko1"});

    args::ValueFlag<double> ra0(parser, "ra0", "0 order reaction rate of nitrification", {"ra0"});
    args::ValueFlag<double> kab1(parser, "kab1", "1 order reaction rate constant of nitrification", {"kab1"});
    args::ValueFlag<double> foxmin(parser, "foxmin", "limit of oxygen of nitrification", {"foxmin"});
    args::ValueFlag<double> c_oxc(parser, "c_oxc", "critical oxygen of nitrification", {"c_oxc"});
    args::ValueFlag<double> c_oxo(parser, "c_oxo", "optimal oxygen of nitrification", {"c_oxo"});
    args::ValueFlag<double> theta_a(parser, "theta_a", "temperatural coefficient of nitrification", {"theta_a"});
    args::ValueFlag<double> T_c(parser, "T_c", "critical temperature of nitrification", {"T_c"});

    args::ValueFlag<double> rn0(parser, "rn0", "0 order reaction rate of denitrification", {"rn0"});
    args::ValueFlag<double> knb1(parser, "knb1", "1 order reaction rate constant of denitrification", {"knb1"});
    args::ValueFlag<double> Tnc(parser, "Tnc", "critical temperature of denitrification", {"Tnc"});
    args::ValueFlag<double> theta_n(parser, "theta_n", "temperatural coefficient of denitrification", {"theta_n"});
    args::ValueFlag<double> c_noxc(parser, "c_noxc", "critical oxygen of denitrification", {"c_noxc"});
    args::ValueFlag<double> c_noxo(parser, "c_noxo", "optimal oxygen of denitrification", {"c_noxo"});
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
    if(filein){
        filename_in = args::get(filein);
    }
    if(fileout){
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

int main(int argc, char *argv[]) {
    std::string filename_in, filename_out;
    std::optional<double> value[15];
    parse(argc, argv, filename_in, filename_out, value);

    write_process_params(filename_in, "process_lib", value[0], value[1], value[2],
                         value[3], value[4], value[5], value[6], value[7], value[8],
                         value[9], value[10], value[11], value[12], value[13], value[14]);
    simulate(filename_in, filename_out);
}