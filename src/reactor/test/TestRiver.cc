#include "../include/River.h"
#include <iostream>

int main() {
    River baihe("../../bai_river.txt");
    while (baihe.flush()) {
        std::cout << "flow:  " << baihe.get_flow()
                  << "\torganic:  " << baihe.get_load_organic()
                  << "\tammonia:  " << baihe.get_load_ammonia()
                  << "\tnitrate:  " << baihe.get_load_nitrate() << std::endl;
    }
}