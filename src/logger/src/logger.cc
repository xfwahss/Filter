#include <logger.h>

std::shared_ptr<spdlog::logger> logger::get(const std::string &logger_name, const std::string &filename) {
    auto console = spdlog::get(logger_name);
    if (console) {
        return console;
    } else {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern("[console][%^%l%$] %v");
        if (!filename.empty()) {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
            file_sink->set_level(spdlog::level::trace);
            auto logger =
                std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list{console_sink, file_sink});
            spdlog::register_logger(logger);
        } else {
            auto logger = std::make_shared<spdlog::logger>(logger_name, spdlog::sinks_init_list{console_sink});
            spdlog::register_logger(logger);
        }
        spdlog::get(logger_name)->set_level(spdlog::level::info);
        // spdlog::get(logger_name)->flush_on(spdlog::level::debug);
        spdlog::get(logger_name)->info("Logger <{}> was created and registered global.", logger_name);
        return spdlog::get(logger_name);
    }
}

void logger::log_vectorxd(const std::string &msg, const Eigen::VectorXd &vec, const std::string &logger_name, const std::string &filename){
    auto logger_use = logger::get(logger_name, filename);
    std::stringstream ss;
    ss << "[" << vec.transpose() << "]";
    logger_use->debug(msg, ss.str());
}


void logger::log_matrixxd(const std::string &msg, const Eigen::MatrixXd &mat, const std::string &logger_name, const std::string &filename){
    auto logger_use = logger::get(logger_name, filename);
    std::stringstream ss;
    ss <<"\n[" << mat << "]";
    logger_use->debug(msg, ss.str());
}