#include "../include/RiverSystem.h"

int main(){
    River a;
    RiverSystem rivers_in;
    rivers_in.add_river("Bai", a);
    rivers_in.add_river("Chao", a);
    std::cout << rivers_in<< std::endl;
}