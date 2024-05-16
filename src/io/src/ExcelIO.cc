#include "../include/ExcelIO.h"
#include <exception>
#include <iostream>

ExcelIO::ExcelIO(const std::string &filename, const std::string &mode)
    : FileIO(filename, mode), mode(mode) {
    std::string extension = FileIO::get_file_type();
    if (extension != "xlsx") {
        throw std::invalid_argument("Wrong type of the filename");
    } else {
        if (mode == "w") {
            file.create(filename);
        }
        if (mode == "r") {
            file.open(filename);
        }
    }
}
ExcelIO::~ExcelIO() {
    if (mode == "w") {
        file.save();
        file.close();
    }
    if (mode == "r") {
        file.close();
    }
}

bool ExcelIO::is_string_invector(const std::string &str,
                                 const std::vector<std::string> &vec) {
    std::vector<std::string>::const_iterator it =
        std::find(vec.begin(), vec.end(), str);
    return it != vec.end();
}
void ExcelIO::read() {}
void ExcelIO::write() {}

int ExcelIO::get_rows(const std::string &sheet_name) {
    auto wks = file.workbook().worksheet(sheet_name);
    int rows = wks.lastCell().row();
    return rows;
}

int ExcelIO::get_columns(const std::string &sheet_name) {
    auto wks    = file.workbook().worksheet(sheet_name);
    int columns = wks.lastCell().column();
    return columns;
}

Eigen::VectorXd ExcelIO::read_row(const std::string &sheet_name,
                                  const int &row) {
    int nums = get_columns(sheet_name);
    auto wks = file.workbook().worksheet(sheet_name);
    Eigen::VectorXd value(nums);
    for (int i = 0; i < nums; ++i) {
        double v = wks.cell(row, i + 1).value();
        value(i) = v;
    }
    return value;
}

void ExcelIO::write_row(const Eigen::VectorXd &value,
                        const std::string &sheet_name, const int &row) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(row, i + 1).value() = value(i);
    }
}

void ExcelIO::write_header(const std::vector<std::string> &value,
                           const std::string &sheet_name, const int &row) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(row, i + 1).value() = value[i];
    }
}

void ExcelIO::write_column(const Eigen::VectorXd &value,
                           const std::string &sheet_name, const int &column) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(i + 1, column).value() = value(i);
    }
}