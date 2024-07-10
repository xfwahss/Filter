#include <Ammonification.h>

void Ammonification::init(const double &ro0, const double &ko1) {
    status.ro0 = ro0;
    status.ko1 = ko1;
}

void Ammonification::update(const ammon_status &updated_status) {
    status = updated_status;
}

double Ammonification::rate(const double &c_no) {
    return status.ro0 + status.ko1 * c_no;
}