#include <DataStructures.h>

int main() {
    test_status b;
    b.a = 9000;
    b.b = 20;
    b.c = 40;
    std::cout << b;
    Eigen::VectorXd c = b;
    Eigen::VectorXd d(3);
    d << 1, 2, 4;
    // test_status e;
    // e = d;
}