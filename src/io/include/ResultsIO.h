#ifndef RESULTS_IO_H
#define RESULTS_IO_H
#include "FileIO.h"
#include <fstream>

class ResultsIO : public FileIO {
  private:
    std::ostream file;

  public:
    ResultsIO(const std::string &filename);
    ~ResultsIO();
    void read() override;
    void write() override;
};
#endif