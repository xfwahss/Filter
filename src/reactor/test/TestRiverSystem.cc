#include <RiverSystem.h>

int main(){
    River a;
    River b;
    RiverSystem rivers_in;
    rivers_in.add_river("Bai", a);
    rivers_in.add_river("Chao", b);
    std::cout << rivers_in<< std::endl;
    Eigen::VectorXd m(8);
    m << 2, 3, 4, 5, 6, 7, 8, 9;
    rivers_in.update(m);
    std::cout << rivers_in.get_status() << std::endl;


    Eigen::VectorXd m2(8);
    m2 << 1, 1, 1, 1, 1, 1, 1, 1;
    rivers_in.update(m2);
    std::cout << rivers_in.get_status() << std::endl;
}