#ifndef EXCELIO_H
#define EXCELIO_H
#include "FileIO.h"
#include <Eigen/Dense>
#include <OpenXLSX.hpp>
#include <map>
#include <vector>

class ExcelIO : public FileIO {
  public:
    ExcelIO(const std::string &filename, const std::string &mode = "r");
    ~ExcelIO();
    // 空白单元格自动填充-999
    Eigen::VectorXd read_row(const std::string&filename, const int &index);
    /* @brief VectorXd 写入指定sheet的指定行
     * @param value 向量VectorXd
     * @param sheet_name 表单名
     * @param row 行号
     */
    void write_row(const Eigen::VectorXd &value, const std::string &sheet_name,
                   const int &row);
    void write_header(const std::vector<std::string> &value,
                      const std::string &sheet_name, const int &row);
    void write_column(const Eigen::VectorXd &value,
                      const std::string &sheet_name, const int &column);
    // 对于读的数据A1，A2单元格分别存储实际存储数据的行数的列数
    int get_rows(const std::string& sheet_name);
    int get_columns(const std::string& sheet_name);
    void read() override;
    void write() override;

  private:
    OpenXLSX::XLDocument file;
    std::string mode;
    bool is_string_invector(const std::string &str,
                            const std::vector<std::string> &vec);
};
#endif