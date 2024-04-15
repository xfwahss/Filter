#include "../include/Denitrification.h"
#include<iostream>

int main(){
    Denitrification denitritor;
    denitritor.init(0.1, 0.01, 5, 1.5, 10, 7, 1.0);
    double rate = denitritor.rate(7, 10, 2.5);
    std::cout<< rate << std::endl;
    denitrification_status up_status(0.1, 0.02, 10, 1.5, 12, 8);
    denitritor.update_status(up_status);
    rate = denitritor.rate(7, 10, 2.5);
    std::cout<< rate << std::endl;


}