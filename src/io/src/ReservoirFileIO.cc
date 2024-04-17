#include "../include/ReservoirFileIO.h"

ReservoirFileIO::ReservoirFileIO(const std::string &filename,
                                 const std::string &mode)
    : FileIO(filename, mode) {
    if (mode == "r") {
        ifile.open(FileIO::get_filename());
    } else if (mode == "w") {
        ofile.open(FileIO::get_filename());
    }
}

ReservoirFileIO::~ReservoirFileIO() {
    if (ifile.is_open()) {
        ifile.close();
    };
    if (ofile.is_open()) {
        ofile.close();
    };
}

void ReservoirFileIO::read(){};
void ReservoirFileIO::write() {}
void ReservoirFileIO::writeline(const reservoir_status &ress) {
    ofile << ress.wl << "\t" << ress.c_na << "\t" << ress.c_nn << "\t"
          << ress.c_nn + ress.c_na + ress.c_no << std::endl;
}