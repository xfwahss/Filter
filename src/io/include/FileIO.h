#ifndef FILE_IO_H
#define FILE_IO_H
#include <string>
class FileIO {
  private:
    std::string filename;

  protected:
    std::string get_filename();

  public:
    FileIO(const std::string &filename);
    ~FileIO();
    virtual void read() = 0;
    virtual void write() = 0;
    // 判断读取到的行是不是注释
    bool is_comment(const std::string &line);
    // 判断读取到的行是否为空白行
    bool is_blank(const std::string &line);
};
#endif