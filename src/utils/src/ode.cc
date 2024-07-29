#include <ode.h>

/* @breif 四阶龙格库塔求解器 dc/dt = f(c, t)
 *  @param f，是求解变量和时间的函数
 *  @param t， 当前时间步的时间
 * @param c， 当前时间步的变量值
 * @param dt, 向前推进dt时间步
 * @return 返回 t + dt时的变量值
 */
double
ODE::rungekutta4(std::function<double(const double &t, const double &c)> f,
                 const double &t, const double &c, const double &dt) {
    double k1 =  f(t, c);
    double k2 =  f(t + 0.5 * dt, c + 0.5 * k1 * dt);
    double k3 =  f(t + 0.5 * dt, c + 0.5 * k2 * dt);
    double k4 =  f(t + dt, c + k3 * dt);
    return c + (k1 + 2 * k2 + 2 * k3 + k4) / 6.0 * dt;
}