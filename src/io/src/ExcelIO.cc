#include "../include/ExcelIO.h"
#include <exception>

ExcelIO::ExcelIO(const std::string &filename, const std::string &mode)
    : FileIO(filename, mode) {
    std::string extension = FileIO::get_file_type();
    if (extension != "xlsx") {
        throw std::invalid_argument("Wrong type of the filename");
    }
    if (mode == "w") {
        file.create(filename);
    }
}
ExcelIO::~ExcelIO() {}
void ExcelIO::read() {}
void ExcelIO::write() {}

Eigen::VectorXd ExcelIO::read_row(const int &index) {
    Eigen::VectorXd value;
    return value;
}

Eigen::VectorXd ExcelIO::read_column(const int &index) {
    Eigen::VectorXd value;
    return value;
}

void ExcelIO::write_row(const Eigen::VectorXd &value, const int &row) {}

void ExcelIO::write_column(const Eigen::VectorXd &value, const int &column) {}