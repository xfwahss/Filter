#include <FilterIO.h>
#include <logger.h>

// 负责状态观测步，用于将观测数据由文件读入转化为均值和方差
// 初始状态读取 和 相关向量的给定
FilterIO::FilterIO(const std::string &filename_in, const std::string &filename_out)
    : file_in(filename_in, "r"), file_out(filename_out, "w"), index(0) {
    total_nums = get_params("Params", 1, 2, 1)["measurement_nums"];
};
FilterIO::~FilterIO(){};
void FilterIO::increment_index() { ++index; }
int FilterIO::get_index() { return index; }

Eigen::VectorXd FilterIO::get_init_X(const std::string &sheet_name, const int &column, const int &start_row,
                                     const int &element_nums) {
    // 第一列存储变量的介绍，第二列存储数据
    Eigen::VectorXd X = file_in.read_column(sheet_name, column, start_row, element_nums);
    return X;
}
Eigen::MatrixXd FilterIO::get_init_P(const std::string &sheet_name, const int &start_row, const int &start_column,
                                     const int &nums) {
    int dims          = nums == 0 ? file_in.get_rows(sheet_name) : nums;
    // 从最左上角存储数据
    Eigen::MatrixXd P = file_in.read_block(sheet_name, start_row, start_column, dims, dims);
    return P;
}
Eigen::MatrixXd FilterIO::get_matrix(const std::string &sheet_name, const int &start_row, const int &start_col,
                                     const int &rows, const int &cols) {
    return file_in.read_block(sheet_name, start_row, start_col, rows, cols);
}
Eigen::MatrixXd FilterIO::get_H(const std::string &sheet_name, const int &start_row, const int &start_column,
                                const int &rows, const int &columns) {
    int column_nums   = columns == 0 ? file_in.get_columns(sheet_name) : columns;
    int row_nums      = rows == 0 ? file_in.get_rows(sheet_name) : rows;
    Eigen::MatrixXd H = file_in.read_block(sheet_name, start_row, start_column, row_nums, column_nums);
    return H;
}
Eigen::VectorXd FilterIO::get_Q(const std::string &sheet_name, const int &column, const int &start_row,
                                const int &element_nums) {
    // 第一列存储变量的介绍，第二列存储数据, 给单个预测粒子添加方差
    Eigen::VectorXd Q = file_in.read_column(sheet_name, column, start_row, element_nums);
    return Q;
}

std::unordered_map<std::string, double> FilterIO::get_params(const std::string &sheet_name, const int &index_column,
                                                             const int &value_column, const int &start_row) {
    std::unordered_map<std::string, double> params;
    if (file_in.read_cell_string(sheet_name, start_row, index_column) != "param_nums") {
        throw std::invalid_argument("Please set the number of parameters to be "
                                    "read as the first parameter, format as 'param_nums 1'");
    } else {
        int element_nums = file_in.read_column(sheet_name, value_column, start_row, 1)(0);
        for (int i = 1; i < element_nums + 1; ++i) {
            std::string index = file_in.read_cell_string(sheet_name, i + start_row, index_column);
            params[index]     = file_in.read_column(sheet_name, value_column, start_row + i, 1)(0);
        }
    }
    return params;
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
    file_out.remove_sheet("Sheet1");
}
// 记录同化的数据结果
void FilterIO::write_x(const Eigen::VectorXd &x, const std::string &sheet_name, const int &start_row,
                       const int &start_column) {
    file_out.write_row(x, sheet_name, index + start_row, start_column);
}
void FilterIO::write_P(const Eigen::MatrixXd &P, const std::string &sheet_name, const int &start_row,
                       const int &start_column) {
    int nums = P.cols();
    Eigen::VectorXd values(nums);
    for (int i = 0; i < nums; ++i) {
        values(i) = P(i, i);
    }
    file_out.write_row(values, sheet_name, index + start_row, start_column);
}

void FilterIO::read(Eigen::VectorXd &z, Eigen::MatrixXd &R) {

    if (index < total_nums) {

        logger::get()->info("Processing measurement of index: {}", index + 1);
        // std::cout << "Processing measurement value of index: " << index + 1
        //   << std::endl;
        read_one(file_in, z, R, index);
        increment_index();
    } else {
        z = Eigen::VectorXd(0);
        R = Eigen::MatrixXd(0, 0);
    }
}

void FilterIO::read_one(ExcelIO &file, Eigen::VectorXd &z, Eigen::MatrixXd &R, const int &index) {
    z                            = file.read_row("Obs", index + 2, 2);
    Eigen::VectorXd vector_rread = file.read_row("R", index + 2, 2);
    Eigen::MatrixXd r_read       = Eigen::MatrixXd::Zero(vector_rread.size(), vector_rread.size());
    for (int i = 0; i < vector_rread.size(); ++i) {
        r_read(i, i) = vector_rread(i);
    }
    R = r_read;
}
