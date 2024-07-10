#include <River.h>
#include <logger.h>

int River::status_init_dims = 4;
River::River(){};
River::~River(){};
void River::init(const double &flow, const double &c_no, const double &c_na, const double &c_nn) {
    river_status status(flow, c_no, c_na, c_nn);
    this->status = status;
}

river_status River::get_status() { return status; }

void River::update(river_status &status) { this->status = status; }
void River::update(const Eigen::VectorXd &status) {
    if (status.size() != status_init_dims) {
        logger::get("river")->error("The size of Eigen::VectorXd used to update River status is not equal to {}",
                                    status_init_dims);
    } else{
        river_status s; 
        s= status;
        update(s);
    }
}

void River::predict(const double &dt, const double &d_flow, const double &d_cno, const double &d_cna,
                    const double &d_cnn) {
    if (dt == 1) {
        double next_flow = status.flow + d_flow;
        double next_cno  = status.c_no + d_cno;
        double next_cna  = status.c_na + d_cna;
        double next_cnn  = status.c_nn + d_cnn;
        river_status status(next_flow, next_cno, next_cna, next_cnn);
        update(status);
    } else {
        throw "Not implement for dt!=1";
    }
}
