#include <ExcelIO.h>

ExcelIO::ExcelIO(const std::string &filename, const std::string &mode) : FileIO(filename, mode), mode(mode) {
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

DataVariant ExcelIO::read_cell(const std::string &sheet_name, const int &row,
                                                                       const int &column) const {
    auto ws                     = file.workbook().worksheet(sheet_name);
    OpenXLSX::XLCellValue value = ws.cell(row, column).value();
    DataVariant v;
    switch (value.type()) {
    case OpenXLSX::XLValueType::Empty:
        v = std::string("null");
        break;
    case OpenXLSX::XLValueType::Boolean:
        v = value.get<bool>();
        break;
    case OpenXLSX::XLValueType::Integer:
        v = value.get<int>();
        break;
    case OpenXLSX::XLValueType::Float:
        v = value.get<float>();
        break;
    case OpenXLSX::XLValueType::Error:
        v = std::string("error");
        break;
    case OpenXLSX::XLValueType::String:
        v = value.get<std::string>();
        break;
    }
    return v;
}

bool ExcelIO::is_string_invector(const std::string &str, const std::vector<std::string> &vec) {
    std::vector<std::string>::const_iterator it = std::find(vec.begin(), vec.end(), str);
    return it != vec.end();
}

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

double ExcelIO::get_cell_value(const std::string &sheet_name, const int &row, const int &column) {
    // int max_rows    = get_rows(sheet_name);
    // int max_columns = get_columns(sheet_name);
    // if (row > max_rows) {
    //     throw std::invalid_argument("Max rows were exceeded");
    // } else if (column > max_columns) {
    //     throw std::invalid_argument("Max columns were exceeded");
    // }
    auto wks               = file.workbook().worksheet(sheet_name);
    std::string value_type = wks.cell(row, column).value().typeAsString();
    if (value_type == "empty") {
        return -999;
    } else if (value_type == "integer") {
        int v = wks.cell(row, column).value();
        return double(v);
    } else if (value_type == "float") {
        float v = wks.cell(row, column).value();
        return double(v);
    } else {
        std::stringstream ss;
        ss << "Expected int or float, but received:" << value_type << " ,at row:" << row << ", column:" << column
           << std::endl;
        throw std::invalid_argument(ss.str());
    }
}

Eigen::VectorXd ExcelIO::read_row(const std::string &sheet_name, const int &row, const int &start_column,
                                  const int &end_columns) {
    int nums = (end_columns == -1) ? get_columns(sheet_name) : end_columns;
    Eigen::VectorXd value(nums - start_column + 1);
    for (int i = start_column; i < nums + 1; ++i) {
        value(i - start_column) = get_cell_value(sheet_name, row, i);
    }
    return value;
}

Eigen::VectorXd ExcelIO::read_column(const std::string &sheet_name, const int &column, const int &start_row,
                                     const int &element_nums) {
    int nums = element_nums == 0 ? get_rows(sheet_name) : element_nums;
    Eigen::VectorXd value(nums);
    for (int i = 0; i < nums; ++i) {
        value(i) = get_cell_value(sheet_name, i + start_row, column);
    }
    return value;
}

Eigen::MatrixXd ExcelIO::read_block(const std::string &sheet_name, const int &start_row, const int &start_column,
                                    const int &rows, const int &columns) {
    Eigen::MatrixXd value(rows, columns);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            value(i, j) = get_cell_value(sheet_name, i + start_row, j + start_column);
        }
    }
    return value;
}

std::string ExcelIO::read_cell_string(const std::string &sheet_name, const int &row, const int &column) {
    auto wks      = file.workbook().worksheet(sheet_name);
    std::string s = wks.cell(row, column).value();
    return s;
}

void ExcelIO::remove_sheet(const std::string &sheet_name) { file.workbook().deleteSheet(sheet_name); }

void ExcelIO::write_row(const Eigen::VectorXd &value, const std::string &sheet_name, const int &row,
                        const int &start_column) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(row, i + start_column).value() = value(i);
    }
}

void ExcelIO::write_header(const std::vector<std::string> &value, const std::string &sheet_name, const int &row) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(row, i + 1).value() = value[i];
    }
}

void ExcelIO::write_column(const Eigen::VectorXd &value, const std::string &sheet_name, const int &column) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks = file.workbook().worksheet(sheet_name);
    int nums = value.size();
    for (int i = 0; i < nums; ++i) {
        wks.cell(i + 1, column).value() = value(i);
    }
}

void ExcelIO::write_cell_string(const std::string &value, const std::string &sheet_name, const int &row,
                                const int &column) {
    if (!is_string_invector(sheet_name, file.workbook().sheetNames())) {
        file.workbook().addWorksheet(sheet_name);
    }
    auto wks                      = file.workbook().worksheet(sheet_name);
    wks.cell(row, column).value() = value;
}

std::unordered_map<std::string, double> ExcelIO::read_dict(const std::string &sheet_name, const int &index_column,
                                                           const int &value_column, const int &start_row) {
    std::unordered_map<std::string, double> dicts;
    if (read_cell_string(sheet_name, start_row, index_column) != "element_nums") {
        throw std::invalid_argument("Please set the number of parameters to be "
                                    "read as the first parameter, format as 'param_nums 1'");
    } else {
        int element_nums = read_column(sheet_name, value_column, start_row, 1)(0);
        for (int i = 1; i < element_nums + 1; ++i) {
            std::string index = read_cell_string(sheet_name, i + start_row, index_column);
            dicts[index]      = read_column(sheet_name, value_column, start_row + i, 1)(0);
        }
    }
    return dicts;
}