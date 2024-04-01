#ifndef MATFILE_IO_H
#define MATFILE_IO_H
#include "FileIO.h"
#include "InitState.h"
#include <Eigen/Dense>
#include <fstream>

class MatFileIO : public FileIO {
  private:
    std::ifstream file;
    InitState initstate;
    // apply by read, for matrix block
    void read_matrix(const std::string &matrixinfo);
    // apply by read, for vector block
    void read_vector(const std::string &vectorinfo);

  public:
    MatFileIO(const std::string &filename);
    ~MatFileIO();
    void read() override;
    void write() override;
    InitState get_initstate();
};
#endif