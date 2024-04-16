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

bool FileIO::is_comment(const std::string&line){
    if (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos){
        return true;
    }else{
        return false;
    }

}

bool FileIO::is_blank(const std::string &line){
    if (line.size() > 0 && line[0] == '#'){
        return true;
    }else{
        return false;
    }

}
FileIO::~FileIO() {}