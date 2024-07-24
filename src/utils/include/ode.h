#ifndef ODE_H
#define ODE_H
#include <functional>

class ODE{
    public:
        static double rungekutta4(std::function<double(double, double)> f, double t, double c, double h);
};

#endif