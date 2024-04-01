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
};