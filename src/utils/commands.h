#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <unordered_map>

namespace tools {
std::unordered_map<std::string, std::string> get_args(int argc, char *argv[]) {
    std::unordered_map<std::string, std::string> options;
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 < argc) {
            // Check if option flag starts with "-"
            if (argv[i][0] == '-') {
                std::string key   = argv[i];
                std::string value = argv[i + 1];
                options[key]      = value;
            }
        }
    }
    return options;
}
} // namespace tools
#endif