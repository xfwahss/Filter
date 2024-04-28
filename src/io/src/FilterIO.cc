#include "../include/FilterIO.h"

FilterIO::FilterIO(const std::string &inputfile, const std::string &outputfile)
    : FileIO(inputfile) {
    ifile.open(FileIO::get_filename());
    this->init_dims();
    ofile.open(outputfile);
}

void FilterIO::init_dims() {
    std::string line;
    std::getline(ifile, line);
    std::istringstream iss(line);
    std::string value;
    int values[2];
    for (int i = 0; i < 2; i++) {
        std::getline(iss, value, ',');
        values[i] = std::stoi(value);
    };
    measurement_nums = values[0];
    columns          = values[1];
}

FilterIO::~FilterIO() {
    ifile.close();
    ofile.close();
}

int FilterIO::get_columns() { return columns; }

int FilterIO::get_measurement_nums() { return measurement_nums; }


void FilterIO::readline_to_matrixxd(Eigen::MatrixXd &measurement) {
    std::string line;
    if (std::getline(ifile, line)) {
        std::istringstream iss(line);
        std::string value;
        for (int i = 0; i < columns; i++) {
            std::getline(iss, value, ',');
            measurement(0, i) = std::stod(value);
        };
    }
};
void FilterIO::readline_to_vectorxd(Eigen::VectorXd &measurement) {
    std::string line;
    if (std::getline(ifile, line)) {
        std::istringstream iss(line);
        std::string value;
        for (int i = 0; i < columns; i++) {
            std::getline(iss, value, ',');
            measurement[i] = std::stod(value);
        };
    }
};
void FilterIO::write_comment(const std::string &comment) {
    ofile << comment << std::endl;
}

void FilterIO::write_vectorxd_diagonal_matrixd(Eigen::VectorXd &status,
                                               Eigen::MatrixXd &covariance) {
    int vector_size = status.size();
    int matrix_dims = covariance.cols();
    for (int i = 0; i < vector_size; i++) {
        ofile << std::fixed << std::setprecision(2) << status[i] << ",";
    }
    for (int j = 0; j < matrix_dims; j++) {
        ofile << std::fixed << std::setprecision(2) << covariance.row(j)[j] << ",";
    }
    ofile << std::endl;
}
void FilterIO::read() {}
void FilterIO::write() {}