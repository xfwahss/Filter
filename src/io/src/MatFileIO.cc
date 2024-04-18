#include "../include/MatFileIO.h"

MatFileIO::MatFileIO(const std::string &filename) : FileIO(filename) {
    file.open(FileIO::get_filename());
}

MatFileIO::~MatFileIO() { file.close(); }

void MatFileIO::read_matrix(const std::string &matrixinfo) {
    std::istringstream iss(matrixinfo);
    std::string name;
    int rows, columns;
    iss >> name;
    iss >> rows;
    iss >> columns;
    Eigen::MatrixXd matrix(rows, columns);
    for (int i = 0; i < rows; i++) {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        for (int j = 0; j < columns; j++) {
            double value;
            if (iss >> value) {
                matrix(i, j) = value;
            } else {
                throw std::runtime_error("Matrix:: wrong number of column "
                                         "elements provides, check:" +
                                         name);
            }
        }
    }
    initstate.set_matrix(name, matrix);
}

void MatFileIO::read_vector(const std::string &vectorinfo) {
    // Create a column vector
    std::istringstream iss(vectorinfo);
    std::string name;
    int rows;
    iss >> name;
    iss >> rows;
    Eigen::VectorXd vector(rows);
    std::string line;
    std::getline(file, line);
    std::istringstream rowsiss(line);
    for (int i = 0; i < rows; i++) {
        double value;
        if (rowsiss >> value) {
            vector(i) = value;
        } else {
            throw std::runtime_error(
                "Vector::Wrong number of data provided, check!!!");
        }
    }
    initstate.set_vector(name, vector);
}

void MatFileIO::read() {
    std::string line;
    while (std::getline(file, line)) {
        if (is_blank(line) || is_comment(line)) {
            continue;
        } else {
            std::istringstream iss(line);
            std::string mattype;
            iss >> mattype;
            std::ostringstream oss;
            oss << iss.rdbuf();
            std::string info = oss.str();
            if (mattype == "matrix") {
                read_matrix(info);
            } else if (mattype == "vector") {
                read_vector(info);
            } else {
                throw std::invalid_argument("Wrong matrix category in matfile");
            }
        }
    }
}

InitState MatFileIO::get_initstate() { return initstate; }

void MatFileIO::write() {}
