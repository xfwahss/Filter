#ifndef ODE_H
#define ODE_H
#include <functional>

class ODE {
  public:
    static double
    rungekutta4(std::function<double(const double &t, const double &c)> f,
                const double &t, const double &c, const double &dt);
};

#endif