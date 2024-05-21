#ifndef EXCELIO_H
#define EXCELIO_H
#include "FileIO.h"
#include <Eigen/Dense>
#include <OpenXLSX.hpp>
#include <vector>

class ExcelIO : public FileIO {
  public:
    ExcelIO(const std::string &filename, const std::string &mode = "r");
    ~ExcelIO();
    // 空白单元格自动填充-999
    Eigen::VectorXd read_row(const std::string &sheet_name, const int &row,
                             const int &start_column = 1);
    Eigen::VectorXd read_column(const std::string &sheet_name,
                                const int &column, const int &start_row = 1,
                                const int &element_nums = 0);
    Eigen::MatrixXd read_block(const std::string &sheet_name,
                               const int &start_row, const int &start_column,
                               const int &rows, const int &columns);
    std::string read_cell_string(const std::string &sheet_name, const int &row,
                                 const int &column);
    /* @brief VectorXd 写入指定sheet的指定行
     * @param value 向量VectorXd
     * @param sheet_name 表单名
     * @param row 行号
     */
    void write_row(const Eigen::VectorXd &value, const std::string &sheet_name,
                   const int &row, const int &start_column = 1);
    void write_header(const std::vector<std::string> &value,
                      const std::string &sheet_name, const int &row);
    void write_column(const Eigen::VectorXd &value,
                      const std::string &sheet_name, const int &column);
    void write_cell_string(const std::string &value,
                           const std::string &sheet_name, const int &row,
                           const int &column);
    // 对于读的数据A1，A2单元格分别存储实际存储数据的行数的列数
    int get_rows(const std::string &sheet_name);
    int get_columns(const std::string &sheet_name);
    void remove_sheet(const std::string &sheet_name);

  protected:
    // 以double类型读取单元格数据
    double get_cell_value(const std::string &sheet_name, const int &rows,
                          const int &columns);
    bool is_string_invector(const std::string &str,
                            const std::vector<std::string> &vec);

  private:
    OpenXLSX::XLDocument file;
    std::string mode;
};
#endif