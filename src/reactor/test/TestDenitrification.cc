#include <Denitrification.h>
#include<iostream>

int main(){
    Denitrification denitritor;
    denitritor.init(0.1, 0.01, 5, 1.5, 10, 7, 1.0);
    double rate = denitritor.rate(7, 10, 2.5);
    std::cout<< rate << std::endl;
    DenificationStatus up_status(0.1, 0.02, 10, 1.5, 12, 8);
    denitritor.update(up_status);
    rate = denitritor.rate(7, 10, 2.5);
    std::cout<< rate << std::endl;
    Eigen::VectorXd a = static_cast<Eigen::VectorXd>(denitritor.get_status());
    std::cout << a << std::endl;
    Eigen::VectorXd b(6);
    b << 1, 2, 3, 4, 5, 6;
    DenificationStatus newdeni;
    newdeni = b;
    std::cout << newdeni.rn0 << std::endl;
}