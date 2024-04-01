#include "../include/FileIO.h"
#include <filesystem>

FileIO::FileIO(const std::string &filename) : filename(filename) {
    std::filesystem::path filepath(filename);
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("File does not exist: " + filename);  
    }
}

std::string FileIO::get_filename(){
    return filename;
}

FileIO::~FileIO() {}