#include <ExcelIO.h>
int main() {
    ExcelIO file("test/data/ReadCellTest_Data.xlsx", "r");
    for (int i = 0; i < 12; ++i) {
        DataVariant a = file.read_cell("Test", i + 1, 1);
        std::cout << a.dvalue() << std::endl;
    }
}