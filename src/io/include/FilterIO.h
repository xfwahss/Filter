#ifndef FILTER_IO_H
#define FILTER_IO_H
#include "ExcelIO.h"
class FilterIO {
  private:
    ExcelIO file_in;
    ExcelIO file_out;
    int index;

  public:
    FilterIO(const std::string &filename_in, const std::string &filename_out);
    ~FilterIO();
    Eigen::VectorXd get_init_X();
    Eigen::MatrixXd get_init_P();
    Eigen::MatrixXd get_H();
    Eigen::VectorXd get_Q();
    // 读取观测数据，生成观测平均值和协方差, 设置为虚函数，便于重写
    // 要保证z比R先读，读取R时会更新索引
    virtual Eigen::VectorXd next_z(const int &start_index = 2);
    // 与Obs同时同步，不更新index，index由next_R更新
    virtual Eigen::MatrixXd next_R(const int &start_index = 2);
    virtual void get_obs(Eigen::VectorXd &z, Eigen::MatrixXd &R,
                         const int &start_index = 2);
    void write_headers();
    // 记录同化的数据结果
    void write_x(const Eigen::VectorXd &x);
    void write_P(const Eigen::VectorXd &P);
};
#endif