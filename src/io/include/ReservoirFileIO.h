#ifndef RESERVOIRFILEIO_H
#define RESERVOIRFILEIO_H
#include "FileIO.h"
#include<fstream>

struct reservoir_status {
    double wl;   // 水库水位
    double c_no; // 水库有机氮浓度
    double c_na; // 水库氨氮浓度
    double c_nn; // 水库硝态氮浓度
    reservoir_status() = default;
    reservoir_status(const double &wl, const double &c_no, const double &c_na,
                     const double &c_nn)
        : wl(wl), c_no(c_no), c_na(c_na), c_nn(c_nn) {}
};

class ReservoirFileIO : public FileIO {
  private:
    std::ifstream ifile;
    std::ofstream ofile;
  public:
    ReservoirFileIO(const std::string &filename, const std::string &mode);
    ~ReservoirFileIO();
    void read() override;
    void write() override;
    void writeline(const reservoir_status &ress);
};
#endif