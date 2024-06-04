#include "../include/DataStructures.h"
#include<iostream>

int main() {
    test_status b;
    b.a = 9000;
    b.b = 20;
    b.c = 40;
    Eigen::VectorXd c = b;
    std::cout << c << std::endl;
}