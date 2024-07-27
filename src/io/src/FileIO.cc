#include <FileIO.h>
#include <filesystem>
#include <iostream>

FileIO::FileIO(const std::string &filename, const std::string &mode)
    : filename(filename) {
    std::filesystem::path filepath(filename);
    if (mode == "r" && !std::filesystem::exists(filepath)) {
        throw std::runtime_error("File does not exist: " + filename);
    }
}

std::string FileIO::get_filename() { return filename; }

bool FileIO::is_comment(const std::string &line) {
    if (line.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
        return true;
    } else {
        return false;
    }
}

bool FileIO::is_blank(const std::string &line) {
    if (line.size() > 0 && line[0] == '#') {
        return true;
    } else {
        return false;
    }
}

std::string FileIO::get_file_type() {
    size_t dot_position = filename.find_last_of(".");
    if (dot_position == std::string::npos) {
        return "Unknow";
    }
    std::string extension = filename.substr(dot_position + 1);
    return extension;
}
FileIO::~FileIO() {}

void FileIO::copyFile(const std::filesystem::path &src,
              const std::filesystem::path &dest) {
    std::filesystem::path destdir = dest.parent_path();
    if (!std::filesystem::exists(destdir) ||
        !std::filesystem::is_directory(destdir)) {
        try {
            std::filesystem::create_directories(destdir);
        } catch (std::filesystem::filesystem_error &e) {
            std::cerr << "Failed to create directories: " << e.what()
                      << std::endl;
            return;
        }
    }
    try {
        std::filesystem::copy_file(
            src, dest, std::filesystem::copy_options::overwrite_existing);
        std::cout << "File copied successfully." << std::endl;
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Failed to copy file: " << e.what() << std::endl;
    }
}