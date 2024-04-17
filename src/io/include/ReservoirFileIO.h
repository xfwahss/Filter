#ifndef RESERVOIRFILEIO_H
#define RESERVOIRFILEIO_H
#include "FileIO.h"
#include<fstream>
#include"../../reactor/include/Reservoir.h"

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