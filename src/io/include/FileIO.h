#ifndef FILE_IO_H
#define FILE_IO_H
#include <filesystem>
#include <string>

class FileIO {

  private:
    std::string filename;

  protected:
    std::string get_filename();

  public:
    static void copyFile(const std::filesystem::path &src,
                         const std::filesystem::path &dest);
    FileIO(const std::string &filename, const std::string &mode = "r");
    ~FileIO();
    // 判断读取到的行是不是注释
    bool is_comment(const std::string &line);
    // 判断读取到的行是否为空白行
    bool is_blank(const std::string &line);
    std::string get_file_type();
};
#endif