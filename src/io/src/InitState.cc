#include "../include/InitState.h"

void InitState::set_matrix(const std::string &name,
                           const Eigen::MatrixXd &value) {
    if (name == "P") {
        P = value;
    } else if (name == "F") {
        F = value;
    } else if (name == "B") {
        B = value;
    } else if (name == "H") {
        H = value;
    } else if (name == "Q") {
        Q = value;
    } else if (name == "R") {
        R = value;
    } else {
        throw std::runtime_error("Unexpected init state variable specifyed");
    }
}

void InitState::set_vector(const std::string &name,
                           const Eigen::VectorXd &value) {
    if (name == "x") {
        x = value;
    } else if (name == "u") {
        u = value;
    } else {
        throw std::runtime_error("Unexpected init state variable specifyed");
    }
}

Eigen::MatrixXd InitState::get_matrix(const std::string &name) {
    if (name == "P") {
        return P;
    } else if (name == "F") {
        return F;
    } else if (name == "B") {
        return B;
    } else if (name == "H") {
        return H;
    } else if (name == "Q") {
        return Q;
    } else if (name == "R") {
        return R;
    } else {
        throw std::runtime_error("Unexpected init state variable to get");
    }
}

Eigen::VectorXd InitState::get_vector(const std::string &name){
    if (name == "x"){
        return x;
    } else if (name =="u"){
        return u;
    } else {
        throw std::runtime_error("Unexpected init state variable to get");
    }
}