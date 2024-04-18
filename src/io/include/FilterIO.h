#ifndef FILTER_IO_H
#define FILTER_IO_H
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>

class FilterIO {
  private:
    std::ifstream file;
    int columns;
    int measurement_nums;
    void init_dims();

  public:
    FilterIO(const std::string &filename);
    ~FilterIO();
    void readline_to_vectorxd(Eigen::VectorXd &measurement);
    void read_init_cond(Eigen::MatrixXd &P);
    int get_columns();
    int get_measurement_nums();
};

#endif