#include <umath.h>
#include <iostream>
#include <vector>
#include <random>

int main() {

    std::random_device rd;
    std::cout << rd() << std::endl;
    for(int i=0; i<20; ++i){
        std::cout << umath::randomd(0, 1, 40)<<std::endl;
    }
    Eigen::VectorXd a(3);
    a << 1.0, 2, 3;
    Eigen::VectorXd b(4);
    b << -2, 1, 3, 4;
    std::cout << "Expected: true, false";
    std::cout << umath::is_positive(a) << "," << umath::is_positive(b)
              << std::endl;

    Eigen::VectorXd mean(3);
    mean << 1, 1, 1;
    std::cout << mean << std::endl;
    Eigen::MatrixXd cov(3, 3);
    cov << 0.09, 0, 0, 0, 0.09, 0, 0, 0, 0.09;

    int nums = 5000;
    std::vector<Eigen::VectorXd *> va(nums);
    for(int i=0; i < nums; ++i){
        Eigen::VectorXd *v = new Eigen::VectorXd(3);
        va[i] = v;
        *v = umath::pos_multi_gauss_random(mean, cov);
    }

    Eigen::VectorXd calc_mean(3);
    calc_mean << 0, 0, 0;
    for(int i=0; i<va.size(); ++i){
        calc_mean += *(va[i]);
    }
    calc_mean = calc_mean / nums;
    std::cout << calc_mean << std::endl;

    Eigen::MatrixXd cal_cov = Eigen::MatrixXd::Zero(3, 3);
    for(int i=0; i<va.size(); ++i){
        cal_cov += (*(va[i]) - calc_mean) * (*(va[i]) - calc_mean).transpose();
    }
    cal_cov = cal_cov / (nums - 1);
    std::cout << cal_cov << std::endl;
}