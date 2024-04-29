/*!
 * @file DataStructues.h
 * @brief 存储各系统组件的的状态数据结构，并实现和Eigen::VectorXd的互转
 * 实现了赋值运算符重载
 * @author xfwahss
 * @version main-v0.1
 * @date 2024-04-28
 */
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H
#include <Eigen/Dense>
#include <stdexcept>

// 存储水库状态
struct reervoir_status {};

struct rier_status {};

struct ammon_status {};

struct nitri_status {
    double ra0;
    double kab1;
    double foxmin;
    double c_oxc;
    double c_oxo;
    double theta_a;
    double T_c;
    nitri_status() = default;
    /* @brief 创建并初始化硝化系统状态
     *  @param ra0 零阶反映速率
     *  @param kab1 20度时的一阶反应速率常数
     *  @param foxmin 最小氧限制系数
     *  @param c_oxc 临界溶解氧浓度
     *  @param c_oxo 最佳溶解氧浓度
     *  @param theta_a 温度系数
     *  @param T_c 临界温度
     */
    nitri_status(const double &ra0, const double &kab1, const double &foxmin,
                 const double &c_oxc, const double &c_oxo,
                 const double &theta_a, const double &T_c)
        : ra0(ra0), kab1(kab1), foxmin(foxmin), c_oxc(c_oxc), c_oxo(c_oxo),
          theta_a(theta_a), T_c(T_c){};
    nitri_status &operator=(const Eigen::VectorXd &v) {
        if (v.size() == 7) {
            ra0     = v(0);
            kab1    = v(1);
            foxmin  = v(2);
            c_oxc   = v(3);
            c_oxo   = v(4);
            theta_a = v(5);
            T_c     = v(6);
            return *this;
        } else {
            throw std::length_error(
                "Length of VectorXd does not match struct:nitri_status");
        }
    }
    operator Eigen::VectorXd() const {
        Eigen::VectorXd v(7);
        v << ra0, kab1, foxmin, c_oxc, c_oxo, theta_a, T_c;
        return v;
    }
};

struct deni_status {
    double rn0;
    double knb1;
    double Tnc;
    double theta_n;
    double c_noxc;
    double c_noxo;
    deni_status() = default;
    /* 创建并初始化反硝化系统状态
     * @param rn0 零阶反硝化速率基值
     * @param knb1 一阶反硝化速率常数
     * @param Tnc 临界温度
     * @param theta_n 温度系数
     * @param c_noxc 临界溶解氧浓度
     * @param c_noxo 最优溶解氧浓度
     */
    deni_status(const double &rn0, const double &knb1, const double &Tnc,
                const double &theta_n, const double &c_noxc,
                const double &c_noxo)
        : rn0(rn0), knb1(knb1), Tnc(Tnc), theta_n(theta_n), c_noxc(c_noxc),
          c_noxo(c_noxo){};
    deni_status &operator=(const Eigen::VectorXd &v) {
        if (v.size() == 6) {
            rn0     = v(0);
            knb1    = v(1);
            Tnc     = v(2);
            theta_n = v(3);
            c_noxc  = v(4);
            c_noxo  = v(5);
            return *this;
        } else {
            throw std::length_error(
                "Length of VectorXd does not match struct:deni_status");
        }
    };
    operator Eigen::VectorXd() const {
        Eigen::VectorXd v(6);
        v << rn0, knb1, Tnc, theta_n, c_noxc, c_noxo;
        return v;
    }
};
#endif