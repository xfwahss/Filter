#include "../include/RiverFileIO.h"
#include <sstream>
#include <iostream>
#include <string>

RiverFileIO::RiverFileIO(const std::string &filename) : FileIO(filename) {
    file.open(FileIO::get_filename());
    // 初始化数据
    data.flow    = 0;
    data.ammonia = 0;
    data.nitrate = 0;
    data.organic = 0;
}

RiverFileIO::~RiverFileIO() { file.close(); }

bool RiverFileIO::readline() {
    std::string line;
    // 还能继续读取，并且读的字符串是空白或者注释，继续读取
    while (line.empty() && std::getline(file, line)) {
        if (is_comment(line) || is_blank(line)) {
            line.clear();
        }
    }
    if (line.empty()) {
        std::cout << "No lines to read!" << std::endl;
        return false;
    } else {
        std::istringstream iss(line);
        double flow, ammonia, nitrate, total_nitrogen;
        iss >> flow;
        iss >> ammonia;
        iss >> nitrate;
        iss >> total_nitrogen;

        if (flow > 0) {
            data.flow = flow;
        };
        if (ammonia > 0) {
            data.ammonia = ammonia;
        }
        if (nitrate > 0) {
            data.nitrate = nitrate;
        }
        if (total_nitrogen > 0) {
            double organic = total_nitrogen - data.ammonia - data.nitrate;
            if (organic > 0) {
                // 大于0才赋值，否则的话使用上一个状态的值
                data.organic = organic;
            }
        }
        return true;
    }
}
void RiverFileIO::read() {}
void RiverFileIO::write() {}