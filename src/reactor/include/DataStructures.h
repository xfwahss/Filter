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
struct nitri_status {};

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