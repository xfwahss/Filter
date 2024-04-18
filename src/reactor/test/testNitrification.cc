#include "../include/Nitrification.h"
#include <iostream>

int main(){
    Nitrification nitriac;
    nitriac.init(0.1, 0.01, 0.5, 2, 8, 0.8, 5, 0);
    double rate = nitriac.rate(7, 20, 5);
    std::cout << rate << std::endl;
    nitrify_status update_values(0.2, 0.01, 0.5, 2, 8, 0.9, 10);
    nitriac.update_status(update_values);
    rate = nitriac.rate(7, 20, 5);
    std::cout << rate << std::endl;
}