#include "FilterIO.h"

FilterIO::FilterIO(const std::string& filename): file(filename){
  if (!file.is_open()) {  
    throw std::runtime_error("Failed to open file: " + filename);  
  }  
  this->init_dims();
}

void FilterIO::init_dims(){
  std::string line;
  std::getline(file, line);
  std::istringstream iss(line);
  std::string value;
  int values[2];
  for (int i=0; i<2; i++){
    std::getline(iss, value, ',');
    values[i] = std::stoi(value);
  };
  measurement_nums = values[0];
  columns = values[1];
}

FilterIO::~FilterIO(){
    file.close();
}

int FilterIO::get_columns(){
  return columns;
}

int FilterIO::get_measurement_nums(){
    return measurement_nums;
}

void FilterIO::readline_to_vectorxd(Eigen::VectorXd &measurement){
  std::string line;
  if (std::getline(file, line)) {  
    std::istringstream iss(line);
    std::string value;
    for(int i=0; i<columns; i++){
        std::getline(iss, value, ',');
        measurement[i] = std::stod(value);
    };
  }  
};

// int main(){
//     FilterIO io("../test/filterio_test.txt");
//     int columns = io.get_columns();
//     int nums = io.get_measurement_nums();
//     Eigen::VectorXd s(columns);
//     for(int i=0; i<nums; i++){
//         io.readline_to_vectorxd(s);
//         std::cout<<s<<std::endl;
//         break;
//     }
// }