#include "../include/RiverFileIO.h"
#include <iostream>

/*
编译命令：
g++ -o TestRiverFileIO.o TestRiverFileIO.cc "../src/RiverFileIO.cc" "../src/FileIO.cc"
*/

int main() {
    RiverFileIO river("../../test_data/test_river.txt");
    bool status = true;
    while (status) {
        status = river.readline();
        if (status) {
            std::cout << "Flow:  " << river.data.flow
            << "\tammonia:  " << river.data.ammonia 
            << "\tnitrate:  " << river.data.nitrate 
            << "\torganic:  " << river.data.organic 
            << std::endl;
        }
    }
}