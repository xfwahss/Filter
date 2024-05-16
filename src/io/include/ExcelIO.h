#ifndef EXCELIO_H
#define EXCELIO_H
#include "FileIO.h"
#include <Eigen/Dense>
#include <OpenXLSX.hpp>

class ExcelIO : public FileIO {
  public:
    ExcelIO(const std::string &filename, const std::string &mode = "r");
    ~ExcelIO();
    Eigen::VectorXd read_row(const int &index);
    Eigen::VectorXd read_column(const int &index);
    void write_row(const Eigen::VectorXd &value, const int &row);
    void write_column(const Eigen::VectorXd &value, const int &column);
    void read() override;
    void write() override;

  private:
    OpenXLSX::XLDocument file;
};
#endif