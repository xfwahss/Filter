#ifndef RIVERFILE_H
#define RIVERFILE_H
#include "FileIO.h"
#include <fstream>

class RiverFileIO : public FileIO {
  private:
    std::ifstream file;
    struct river_data{
        double flow;
        double organic;
        double ammonia;
        double nitrate;
    };
  public:
    RiverFileIO(const std::string &filename);
    ~RiverFileIO();
    river_data data;
    // 每次只读取一个值
    void read() override;
    // True 代表读取到值了；
    bool readline();
    void write() override;
};
#endif