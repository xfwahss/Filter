#include "../include/ExcelIO.h"

int main() { 
    ExcelIO exce("../test/output/TestExcelWrite.xlsx", "w"); 
    Eigen::VectorXd x(8);
    Eigen::VectorXd y(7);
    x << 0, 2, 3, 4, 5, 6, 7, 8;
    y << 2, 3, 4, 5, 6, 7, 8;
    std::vector<std::string> s = {"Hello", "Demo"};
    exce.write_row(x, "Sheet1", 3);
    exce.write_row(x, "Sheet2", 3);
    exce.write_row(x, "Sheet2", 4);
    exce.write_row(x, "Sheet3", 3);
    exce.write_row(y, "Sheet3", 5);
    exce.write_header(s, "Test", 1);
    exce.write_column(x, "Demo", 1);
    std::cout << exce.get_rows("Sheet3") << std::endl;
    std::cout << exce.get_columns("Sheet3") << std::endl;
    ExcelIO test_read("../test/data/TestExcelRead.xlsx");
    auto v = test_read.read_row("Sheet1", 1);
    std::cout << v << std::endl;
    v = test_read.read_row("Sheet1", 2);
    std::cout << v << std::endl;
    v = exce.read_row("Sheet3", 4);
    return 0;
}