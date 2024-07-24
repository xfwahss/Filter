#include "ode.h"
#include <iostream>
#include <math.h>

double decayTrue(double t, double c){
    const double k = 0.5;
    return std::exp(-k * t) * c;
}

// 使用龙格库塔法求解给定初值问题的函数  
void solveRK4(std::function<double(double, double)> f, double t0, double c0, double tf, double h, int steps) {  
    double t = t0;  
    double c = c0;  
  
    std::cout << "Time\t\tValue\n";  
    std::cout << t << "\t\t" << c << "\t\t" << c0 << std::endl;  
  
    for (int i = 0; i < steps; ++i) {  
        c = ODE::rungekutta4(f, t, c, h);  
        t += h;  
  
        if (t > tf) break; // 防止超过最终时间  
  
        std::cout << t << "\t\t" << c << "\t\t" << decayTrue(t, c0) << std::endl;  
    }  
}  
  
// 示例函数：dc/dt = -kc  
double decayFunction(double t, double c) {  
    const double k = 0.5; // 示例中的衰减速率  
    return -k * c;  
}  

  
int main() {  
    double t0 = 0.0;  
    double c0 = 1.0;  
    double tf = 5.0;  
    double h = 0.1;  
    int steps = static_cast<int>((tf - t0) / h) + 1; // 计算步数  
    solveRK4(decayFunction, t0, c0, tf, h, steps);  
  
    return 0;  
}