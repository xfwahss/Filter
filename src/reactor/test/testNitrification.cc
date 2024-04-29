#include "../include/Nitrification.h"
#include <iostream>

int main(){
    Nitrification nitriac;
    nitriac.init(0.1, 0.01, 0.5, 2, 8, 0.8, 5, 0);
    double rate = nitriac.rate(7, 20, 5);
    std::cout << rate << std::endl;
    nitri_status update_values(0.2, 0.01, 0.5, 2, 8, 0.9, 10);
    nitriac.update(update_values);
    rate = nitriac.rate(7, 20, 5);
    std::cout << rate << std::endl;

    Eigen::VectorXd v = static_cast<Eigen::VectorXd>(nitriac.get_status());
    std::cout << v << std::endl;
    nitri_status nnn;
    nnn = v;
    std::cout << nitriac.rate(7, 20, 5)<<std::endl;
}