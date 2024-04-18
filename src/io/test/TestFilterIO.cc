#include "../include/FilterIO.h"

int main() {
    FilterIO io("../../test_data/filterio_read_test.txt", "../../test_data/filterio_write_test.txt");
    int columns = io.get_columns();
    int nums    = io.get_measurement_nums();
    Eigen::VectorXd s(columns);
    for (int i = 0; i < nums; i++) {
        io.readline_to_vectorxd(s);
        std::cout << s << std::endl;
    }
    Eigen::VectorXd witer(5);
    witer << 1, 2, 3, 4, 5;
    Eigen::MatrixXd test(5, 5);
    test << 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 1;
    io.write_comment("X, Y, Z, M, N");
    io.write_vectorxd_diagonal_matrixd(witer, test);
}