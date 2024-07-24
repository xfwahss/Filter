#ifndef LOGGER_H
#define LOGGER_H
#include <Eigen/Dense>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class Logger {
  public:
    static std::vector<std::string> logger_set;

    static std::shared_ptr<spdlog::logger> get(const std::string &logger_name = "console",
                                               const std::string &filename    = std::string());

    static void log_vectorxd(const std::string &msg, const Eigen::VectorXd &vec,
                             const std::string &logger_name = "console", const std::string &filename = std::string());

    static void log_matrixxd(const std::string &msg, const Eigen::MatrixXd &mat,
                             const std::string &logger_name = "console", const std::string &filename = std::string());

    static void set_level(const std::string &name, const std::string &level);
};

namespace logger {
std::shared_ptr<spdlog::logger> get(const std::string &logger_name = "console",
                                    const std::string &filename    = std::string());
void log_vectorxd(const std::string &msg, const Eigen::VectorXd &vec, const std::string &logger_name = "console",
                  const std::string &filename = std::string());

void log_matrixxd(const std::string &msg, const Eigen::MatrixXd &mat, const std::string &logger_name = "console",
                  const std::string &filename = std::string());

} // namespace logger
#endif