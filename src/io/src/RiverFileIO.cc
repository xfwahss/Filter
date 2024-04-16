#include "../include/RiverFileIO.h"
#include <string>
#include <iostream>

RiverFileIO::RiverFileIO(const std::string &filename) : FileIO(filename) {
    file.open(FileIO::get_filename());
}

RiverFileIO::~RiverFileIO() { file.close(); }


bool RiverFileIO::readline(){
    std::string line;
    if(std::getline(file, line)){
        std::cout<<line<<std::endl;
        return true;
    } else{
        return false;
    }
}
void RiverFileIO::read() {}
void RiverFileIO::write() {}