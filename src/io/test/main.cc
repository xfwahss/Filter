#include "../include/MatFileIO.h"
#include<iostream>

int main(){
    MatFileIO matio("../../test/matrix.txt");
    matio.read();
    InitState init;
    init = matio.get_initstate();
    std::cout<<"P:"<<std::endl;
    std::cout<<init.get_matrix("P")<<std::endl;
    std::cout<<"F:"<<std::endl;
    std::cout<<init.get_matrix("F")<<std::endl;
    std::cout<<"H:"<<std::endl;
    std::cout<<init.get_matrix("H")<<std::endl;
    std::cout<<"Q:"<<std::endl;
    std::cout<<init.get_matrix("Q")<<std::endl;
    std::cout<<"R:"<<std::endl;
    std::cout<<init.get_matrix("R")<<std::endl;
    std::cout<<"x:"<<std::endl;
    std::cout<<init.get_vector("x")<<std::endl;
    std::cout<<"u:"<<std::endl;
    std::cout<<init.get_vector("u")<<std::endl;

}