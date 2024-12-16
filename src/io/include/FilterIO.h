#ifndef FILTER_IO_H
#define FILTER_IO_H
#include "ExcelIO.h"
#include <unordered_map>
class FilterIO {
  private:
    ExcelIO file_in;
    ExcelIO file_out;
    int index;
    int total_nums;

  public:
    FilterIO(const std::string &filename_in, const std::string &filename_out);
    ~FilterIO();
    void init(const std::string &filename_in, const std::string &filename_out);
    int get_index();
    void increment_index();
    Eigen::VectorXd get_init_X(const std::string &sheet_name = "Init_X", const int &column = 2,
                               const int &start_row = 1, const int &element_nums = 0);

    Eigen::MatrixXd get_init_P(const std::string &sheet_name = "Init_P", const int &start_row = 1,
                               const int &start_column = 1, const int &nums = 0);

    Eigen::MatrixXd get_matrix(const std::string &sheet_name, const int &start_row = 1, const int &start_col = 1,
                               const int &rows = 0, const int &cols = 0);

    Eigen::MatrixXd get_H(const std::string &sheet_name = "H", const int &start_row = 1, const int &start_column = 1,
                          const int &rows = 0, const int &columns = 0);

    Eigen::VectorXd get_Q(const std::string &sheet_name = "Q", const int &column = 2, const int &start_row = 1,
                          const int &element_nums = 0);

    std::unordered_map<std::string, double> get_params(const std::string &sheet_name = "Params",
                                                       const int &index_column = 1, const int &value_column = 2,
                                                       const int &start_row = 1);

    void write_x(const Eigen::VectorXd &x, const std::string &sheet_name = "X", const int &start_row = 2,
                 const int &start_column = 2);

    void write_P(const Eigen::MatrixXd &P, const std::string &sheet_name = "P", const int &start_row = 2,
                 const int &start_column = 2);

    // 读取的数据存入z和R,
    // index表明第index次观测
    // 传入file参数提供ExcelIO的文件读写方法
    virtual void read_one(ExcelIO &file, Eigen::VectorXd &z, Eigen::MatrixXd &R, const int &index);
    void read(Eigen::VectorXd &z, Eigen::MatrixXd &R);
    void write_headers();
    // 记录同化的数据结果
};
#endif