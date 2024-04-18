#include "../include/MatFileIO.h"
#include<iostream>

int main(){
    MatFileIO matio("../../test_data/test_matrix.txt");
    matio.read();
    InitState ins = matio.get_initstate();

    Eigen::VectorXd x = ins.get_vector("x");
    Eigen::VectorXd u = ins.get_vector("u");
    Eigen::MatrixXd P = ins.get_matrix("P");
    Eigen::MatrixXd F = ins.get_matrix("F");
    Eigen::MatrixXd B = ins.get_matrix("B");
    Eigen::MatrixXd H = ins.get_matrix("H");
    Eigen::MatrixXd R = ins.get_matrix("R");
    Eigen::MatrixXd Q = ins.get_matrix("Q");
    std::cout << x << std::endl;
    std::cout << F << std::endl;
}