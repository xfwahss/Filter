/*!
 * @file EnsembleKalmanFilter.h
 * @brief 集合卡尔曼滤波模板
 * 1. 采用高斯分布采样
 * 2. 模板类需要实现update(VectorXd)方法和predict()方法以及获取当前状态的方法，
 *    预测过程使用模型的精确预测，不确定度由集合卡尔曼滤波给定
 * 3. 每次分析步过后都会重新采样, 确保采样后的集合均值和协方差与给定的一致
 * @author xfwahss
 * @version main-v0.2
 * @date 2024-06-04
 */
#ifndef ENSEMBLE_KALMAN_FILTER_H
#define ENSEMBLE_KALMAN_FILTER_H
#include <Eigen/Dense>
#include <FilterBase.h>
#include <FilterIO.h>
#include <umath.h>
#include <vector>

template <class T> class EnsembleKalmanFilter : public FilterBase {
  public:
    EnsembleKalmanFilter(const int &ensemble_size);
    ~EnsembleKalmanFilter();

    void init(Eigen::VectorXd X, Eigen::MatrixXd P, Eigen::MatrixXd H, Eigen::VectorXd Q, const unsigned int &seed = 0,
              const bool &ensure_pos = false);

    // 单步同化
    void step_assimilation(const double &dt, const Eigen::VectorXd &z, const Eigen::MatrixXd &R);
    // 批量同化
    void batch_assimilation(FilterIO *filterio, const double &dt, const Eigen::MatrixXd &R = Eigen::MatrixXd(),
                            bool ensure_pos = false);
    Eigen::VectorXd status();
    Eigen::MatrixXd covariance();

  private:
    // 存储指向集合中子模型的指针
    std::vector<T *> ensemble;
    // 采用单个推进，误差为向量形式,给定每个方差的大小，均值为0，使用正态随机数
    Eigen::VectorXd Q;
    // 观测矩阵H
    Eigen::MatrixXd H;
    // 用来存储先验的状态和协方差
    Eigen::VectorXd prior_X;
    Eigen::MatrixXd prior_P;
    // 存储观测值和先验值的差异,初始化为0
    Eigen::VectorXd diff_obs_prior;
    // 根据给定的方差生成一个状态的预测随机误差
    Eigen::VectorXd predict_error();
    // 随机采样种子
    unsigned int seed;
    // 保证采样的值是正的
    bool ensure_pos;

    // 构建集合
    void construct(const int &ensemble_size);
    // 销毁集合
    void destruct();

    // 状态采样
    void sample();
    // 样本状态预测
    void sample_predict(const double &dt);

    // 利用样本预测估计整体
    void predict(const double &dt);
    void update(Eigen::VectorXd z, Eigen::MatrixXd R);
};

template <class T> EnsembleKalmanFilter<T>::EnsembleKalmanFilter(const int &ensemble_size) {
    ensemble.reserve(ensemble_size);
    construct(ensemble_size);
}

template <class T>
void EnsembleKalmanFilter<T>::step_assimilation(const double &dt, const Eigen::VectorXd &z, const Eigen::MatrixXd &R) {
    predict(dt);
    update(z, R);
}

template <class T>
void EnsembleKalmanFilter<T>::batch_assimilation(FilterIO *filterio, const double &dt, const Eigen::MatrixXd &R,
                                                 bool ensure_pos) {
    Eigen::VectorXd z;
    Eigen::MatrixXd r_file;
    filterio->write_headers();
    if (R.size() == 0) {
        filterio->read(z, r_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, r_file);
            filterio->write_x(X, "X", 1, 2);
            filterio->write_x(prior_X, "prior_X", 1, 2);
            filterio->write_P(P, "P", 1, 2);
            filterio->read(z, r_file);
        }
    } else {
        filterio->read(z, r_file);
        while (z.size() != 0) {
            this->step_assimilation(dt, z, R);
            filterio->write_x(X, "X", 1, 2);
            filterio->write_x(prior_X, "prior_X", 1, 2);
            filterio->write_P(P, "P", 1, 2);
            filterio->read(z, r_file);
        }
    }
}

template <class T> void EnsembleKalmanFilter<T>::construct(const int &ensemble_size) {
    for (int i = 0; i < ensemble_size; i++) {
        T *ptr = new T();
        ensemble.push_back(ptr);
    }
}

template <class T> void EnsembleKalmanFilter<T>::destruct() {
    for (int i = 0; i < ensemble.size(); i++) {
        delete ensemble[i];
    }
    ensemble.clear();
}

template <class T> void EnsembleKalmanFilter<T>::sample() {
    Eigen::VectorXd (*gauss_random)(Eigen::VectorXd &mean, Eigen::MatrixXd &cov, const unsigned int &seed);
    if (ensure_pos) {
        gauss_random = umath::pos_multi_gauss_random;
    } else {
        gauss_random = umath::multivariate_gaussian_random;
    }
    for (int i = 0; i < ensemble.size(); ++i) {
        Eigen::VectorXd random_num = gauss_random(X, P, seed);
        ensemble[i]->update(random_num, X, P, diff_obs_prior);
    }
}

template <class T> void EnsembleKalmanFilter<T>::sample_predict(const double &dt) {
    for (int i = 0; i < ensemble.size(); ++i) {
        ensemble[i]->step(dt);
    }
}

template <class T> void EnsembleKalmanFilter<T>::predict(const double &dt) {
    sample();
    sample_predict(dt);

    // 计算均值向量
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(X.rows());
    for (int i = 0; i < ensemble.size(); ++i) {
        mean += ensemble[i]->get_status() + predict_error();
    }
    mean                /= ensemble.size();

    // 计算协方差矩阵
    Eigen::MatrixXd cov  = Eigen::MatrixXd::Zero(P.rows(), P.cols());
    for (int j = 0; j < ensemble.size(); ++j) {
        // 需要考虑过程预测引起的误差,添加了get_process_error
        Eigen::VectorXd diff  = ensemble[j]->get_status() + predict_error() - mean;
        cov                  += diff * diff.transpose();
    }
    cov     /= ensemble.size() - 1;
    X        = mean;
    P        = cov;
    prior_X  = mean;
    prior_P  = cov;
}

template <class T> void EnsembleKalmanFilter<T>::update(Eigen::VectorXd z, Eigen::MatrixXd R) {
    bool flag = true;
    Eigen::MatrixXd RCalc = R;
    for (int i = 0; i < z.size(); i++) {
        if (z(i) == -999.0) {
            z(i) = 0;
            flag = false;
            RCalc(i, i) = 100000000000;
        }
    }
    if (flag) {
        Eigen::MatrixXd K = P * H.transpose() * (H * P * H.transpose() + R).inverse();
        X                 = X + K * (z - H * X);
        P                 = (Eigen::MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P;
        // 记录观测值和真实值的差异
        diff_obs_prior    = 0.5 * (z - H * X).array() / z.array() + 0.5 * (z - H * X).array() / (H * X).array();
    } else {
        Eigen::MatrixXd K = P * H.transpose() * (H * P * H.transpose() + RCalc).inverse();
        X                 = X + K * (z - H * X);
        P                 = (Eigen::MatrixXd::Identity(P.rows(), P.cols()) - K * H) * P;
        diff_obs_prior = Eigen::VectorXd::Zero(X.size());
    }
    // diff_obs_prior = umath::re(z, R, H*X, H * P * H.transpose(), seed);
    Logger::log_vectorxd("RE: {}", diff_obs_prior, "Ensemble RE");
}

template <class T>
void EnsembleKalmanFilter<T>::init(Eigen::VectorXd X, Eigen::MatrixXd P, Eigen::MatrixXd H, Eigen::VectorXd Q,
                                   const unsigned int &seed, const bool &ensure_pos) {
    FilterBase::X    = X;
    FilterBase::P    = P;
    prior_X          = X;
    prior_P          = P;
    diff_obs_prior   = Eigen::VectorXd::Zero(X.size());
    this->H          = H;
    this->Q          = Q;
    this->seed       = seed;
    this->ensure_pos = ensure_pos;
}

template <class T> EnsembleKalmanFilter<T>::~EnsembleKalmanFilter() { destruct(); }

template <class T> Eigen::VectorXd EnsembleKalmanFilter<T>::predict_error() {
    Eigen::VectorXd error(FilterBase::X.rows());
    for (int i = 0; i < FilterBase::X.rows(); ++i) {
        error(i) = umath::randomd(0.0, Q(i), seed);
    }
    logger::log_vectorxd("sample Q:{}", error);
    return error;
}

template <class T> Eigen::VectorXd EnsembleKalmanFilter<T>::status() { return FilterBase::X; }

template <class T> Eigen::MatrixXd EnsembleKalmanFilter<T>::covariance() { return FilterBase::P; }

// 提供抽象类提供作为集合卡尔曼滤波的模板类基类
class EnsembleModel {
  private:
    // 抽样总体状态
    Eigen::VectorXd Ensemble_status;
    Eigen::MatrixXd Ensemble_P;
    Eigen::VectorXd diff_obs_prior;

    // 个体状态
    Eigen::VectorXd status;

  protected:
    Eigen::VectorXd get_status() const { return status; }
    Eigen::MatrixXd get_ensemble_status() const { return Ensemble_status; }
    Eigen::MatrixXd get_ensemble_P() const { return Ensemble_P; }
    Eigen::VectorXd get_diff_obs_prior() const { return diff_obs_prior; }
    // 状态不变更新
    Eigen::VectorXd constant_predict(const Eigen::VectorXd &s, const double &dt) { return s; }
    /* @brief 假设一阶导数不变的参数更新
     * @param s VectorXd的数据存储方式应该为 [状态， 1阶导数]
     * @param dt 预测时间间隔
     */
    Eigen::Vector2d const_1derivate_predict(const Eigen::Vector2d &s, const double &dt) {
        Eigen::Vector2d next;
        next(1) = s(1);
        next(0) = s(0) + s(1) * dt;
        return next;
    }
    // 二阶导数不变更新
    /* @brief 假设二阶导数不变的参数更新
     * @param s Vector3d的存储格式为 [状态, 一阶导数, 二阶导数]
     * @param dt 预测时间间隔
     */
    Eigen::Vector3d const_2derivate_predict(const Eigen::Vector3d &s, const double &dt) {
        Eigen::Vector3d next;
        next(2) = s(2);
        next(1) = s(1) + s(2) * dt;
        next(0) = s(0) + s(1) * dt + 0.5 * s(2) * dt * dt;
        return next;
    }

  public:
    Eigen::VectorXd get_status() { return status; }
    // 初始化时与整体的均值和协方差一起存储
    void update(Eigen::VectorXd &status, Eigen::VectorXd &Ensemble_status, Eigen::MatrixXd &Ensemble_P,
                Eigen::VectorXd &diff_prior_obs) {
        this->status          = status;
        this->Ensemble_status = Ensemble_status;
        this->Ensemble_P      = Ensemble_P;
        this->diff_obs_prior  = diff_prior_obs;
    }

    void update(Eigen::VectorXd &status) { this->status = status; }

    // 子类必须实现predict的方法
    virtual Eigen::VectorXd predict(const double &dt, const Eigen::VectorXd &status) = 0;
    void step(const double &dt) {
        Eigen::VectorXd predicted_status = predict(dt, status);
        update(predicted_status);
    }
};

class EnsembleIO : public FilterIO {};

#endif
