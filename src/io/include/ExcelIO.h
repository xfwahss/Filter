/*
 * @Description:
 * @version:
 * @Author: xfwahss
 * @Date: 2024-05-17 20:42:42
 * @LastEditors: xfwahss
 * @LastEditTime: 2024-07-12 09:40:00
 */
#ifndef EXCELIO_H
#define EXCELIO_H
#include "FileIO.h"
#include <DataVariant.h>
#include <Eigen/Dense>
#include <OpenXLSX.hpp>
#include <logger.h>
#include <unordered_map>
#include <vector>

class ExcelIO : public FileIO {
  public:
    ExcelIO(const std::string &filename, const std::string &mode = "r");
    ~ExcelIO();
    /*
     *通用单元格读取数据转化为常规使用类型
     * 左上角第一个单元格坐标为(1, 1)
     */
    DataVariant read_cell(const std::string &sheet_name, const int &row, const int &column) const;
    // 空白单元格自动填充-999
    Eigen::VectorXd read_row(const std::string &sheet_name, const int &row, const int &start_column = 1,
                             const int &end_columns = -1);
    Eigen::VectorXd read_column(const std::string &sheet_name, const int &column, const int &start_row = 1,
                                const int &element_nums = 0);
    Eigen::MatrixXd read_block(const std::string &sheet_name, const int &start_row, const int &start_column,
                               const int &rows, const int &columns);
    std::string read_cell_string(const std::string &sheet_name, const int &row, const int &column);
    /* @brief VectorXd 写入指定sheet的指定行
     * @param value 向量VectorXd
     * @param sheet_name 表单名
     * @param row 行号
     */
    void write_row(const Eigen::VectorXd &value, const std::string &sheet_name, const int &row,
                   const int &start_column = 1);
    void write_header(const std::vector<std::string> &value, const std::string &sheet_name, const int &row);
    void write_column(const Eigen::VectorXd &value, const std::string &sheet_name, const int &column);
    void write_cell_string(const std::string &value, const std::string &sheet_name, const int &row, const int &column);
    // 对于读的数据A1，A2单元格分别存储实际存储数据的行数的列数
    int get_rows(const std::string &sheet_name);
    int get_columns(const std::string &sheet_name);
    void remove_sheet(const std::string &sheet_name);

    std::unordered_map<std::string, double> read_dict(const std::string &sheet_name, const int &index_column,
                                                      const int &value_column, const int &start_row = 1);
    /*
     *@breif 读取Excel文件的配置文件,信息放在sheet中
     *@param sheet_name sheet名称，默认是config
     *@param key_column 存放key的列位置
     *@param value_column 存放value的列位置
     *@param header_row 存放一共读取几行(包括header行)
     */
    std::unordered_map<std::string, std::string> read_config(const std::string &sheet_name = "config",
                                                             const int &key_column = 1, const int &value_column = 2,
                                                             const int &header_row = 1);

  protected:
    // 以double类型读取单元格数据
    double get_cell_value(const std::string &sheet_name, const int &rows, const int &columns);
    bool is_string_invector(const std::string &str, const std::vector<std::string> &vec);

  private:
    OpenXLSX::XLDocument file;
    std::string mode;
};
#endif