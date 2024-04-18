#ifndef FILTER_IO_H
#define FILTER_IO_H
#include "FileIO.h"
#include <Eigen/Dense>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
// 用于读取测量数据和输出同化数据结果的类
class FilterIO : public FileIO {
  private:
    std::ifstream ifile;
    std::ofstream ofile;
    int columns;
    int measurement_nums;
    void init_dims();

  public:
    FilterIO(const std::string &inputfile, const std::string &outputfile);
    ~FilterIO();
    // 读取测量值
    void readline_to_vectorxd(Eigen::VectorXd &measurement);
    // 写入同化结果和单个变量的方差(协方差矩阵的对角值)
    void write_vectorxd_diagonal_matrixd(Eigen::VectorXd &status,
                                         Eigen::MatrixXd &covariance);
    void write_comment(const std::string &comment);

    // void read_init_cond(Eigen::MatrixXd &P);
    int get_columns();
    int get_measurement_nums();
    void read() override;
    void write() override;
};

#endif