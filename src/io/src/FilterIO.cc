#include "../include/FilterIO.h"

// 负责状态观测步，用于将观测数据由文件读入转化为均值和方差
// 初始状态读取 和 相关向量的给定
FilterIO::FilterIO(const std::string &filename_in,
                 const std::string &filename_out)
    : file_in(filename_in, "r"), file_out(filename_out, "w") {
    index = 0;
};
FilterIO::~FilterIO(){};
Eigen::VectorXd FilterIO::get_init_X() {
    // 第一列存储变量的介绍，第二列存储数据
    Eigen::VectorXd X = file_in.read_column("Init_X", 2);
    return X;
}
Eigen::MatrixXd FilterIO::get_init_P() {
    // 从最左上角存储数据
    int dims          = file_in.get_rows("Init_X");
    Eigen::MatrixXd P = file_in.read_block("Init_P", 1, 1, dims, dims);
    return P;
}
Eigen::MatrixXd FilterIO::get_H() {
    // 从最左上角存储数据
    int columns       = file_in.get_columns("H");
    int rows          = file_in.get_rows("H");
    Eigen::MatrixXd H = file_in.read_block("H", 1, 1, rows, columns);
    return H;
}
Eigen::VectorXd FilterIO::get_Q() {
    // 第一列存储变量的介绍，第二列存储数据, 给单个预测粒子添加方差
    Eigen::VectorXd Q = file_in.read_column("Q", 2);
    return Q;
}
// 读取观测数据，生成观测平均值和协方差, 设置为虚函数，便于重写
// 要保证z比R先读，读取R时会更新索引
Eigen::VectorXd FilterIO::next_z(const int &start_index) {
    Eigen::VectorXd value;
    if (start_index + index > file_in.get_rows("Obs")) {
        return value;
    } else {
        value = file_in.read_row("Obs", index + start_index, 2);
        return value;
    }
}
// 与Obs同时同步，不更新index，index由next_R更新
Eigen::MatrixXd FilterIO::next_R(const int &start_index) {
    Eigen::MatrixXd value;
    if (start_index + index > file_in.get_rows("R")) {
        return value;
    } else {
        Eigen::VectorXd vector_value =
            file_in.read_row("R", index + start_index, 2);
        value = Eigen::MatrixXd::Zero(vector_value.size(), vector_value.size());
        for (int i = 0; i < vector_value.size(); ++i) {
            value(i, i) = vector_value(i);
        }
        ++index;
        return value;
    }
}
void FilterIO::get_obs(Eigen::VectorXd &z, Eigen::MatrixXd &R,
                              const int &start_index) {
    std::cout << "Processing measurement value of index: " << index << std::endl;
    z = next_z(start_index);
    R = next_R(start_index);
}
void FilterIO::write_headers() {
    int rows = file_in.get_rows("Init_X");
    file_out.write_cell_string("Index", "X", 1, 1);
    file_out.write_cell_string("Index", "P", 1, 1);
    for (int i = 0; i < rows; ++i) {
        std::string s = file_in.read_cell_string("Init_X", i + 1, 1);
        file_out.write_cell_string(s, "X", 1, i + 2);
        file_out.write_cell_string(s, "P", 1, i + 2);
    }
}
// 记录同化的数据结果
void FilterIO::write_x(const Eigen::VectorXd &x) {
    file_out.write_row(x, "X", index + 1, 2);
}
void FilterIO::write_P(const Eigen::VectorXd &P) {
    int nums = P.cols();
    Eigen::VectorXd values(nums);
    for (int i = 0; i < nums; ++i) {
        values(i) = P(i, i);
    }
    file_out.write_row(values, "P", index + 1, 2);
}