#include <DataVariant.h>
#include <logger.h>
double DataVariant::dvalue() {
    int index = data.index();
    switch (index) {
    case 0:
        logger::get("ExcelIO")->error("replace {} with -999.0", std::get<std::string>(data));
        return -999.0;
    case 1:
        return std::get<double>(data);
    case 2:
        return std::get<float>(data);
    case 3:
        return std::get<int>(data);
    case 4:
        return std::get<bool>(data);
    default:
        throw "Unexpected error";
    }
};

float DataVariant::fvalue() {
    int index = data.index();
    switch (index) {
    case 0:
        logger::get("ExcelIO")->error("replace {} with -999.0", std::get<std::string>(data));
        return -999.0;
    case 1:
        return std::get<double>(data);
    case 2:
        return std::get<float>(data);
    case 3:
        return std::get<int>(data);
    case 4:
        return std::get<bool>(data);
    default:
        throw "Unexpected error";
    }
};

int DataVariant::ivalue() {
    int index = data.index();
    switch (index) {
    case 0:
        logger::get("ExcelIO")->error("replace {} with -999.0", std::get<std::string>(data));
        return -999;
    case 1:
        return std::get<double>(data);
    case 2:
        return std::get<float>(data);
    case 3:
        return std::get<int>(data);
    case 4:
        return std::get<bool>(data);
    default:
        throw "Unexpected error";
    }
};

bool DataVariant::bvalue() {
    int index = data.index();
    switch (index) {
    case 0:
        throw "Can't convert string to bool";
    case 1:
        return std::get<double>(data);
    case 2:
        return std::get<float>(data);
    case 3:
        return std::get<int>(data);
    case 4:
        return std::get<bool>(data);
    default:
        throw "Unexpected error";
    }
};

std::string DataVariant::svalue() {
    int index = data.index();
    switch (index) {
    case 0:
        return std::get<std::string>(data);
    case 1:
        return std::to_string(std::get<double>(data));
    case 2:
        return std::to_string(std::get<float>(data));
    case 3:
        return std::to_string(std::get<int>(data));
    case 4:
        return std::to_string(std::get<bool>(data));
    default:
        throw "Unexpected error";
    }
};

DataVariant &DataVariant::operator=(const double &v) {
    this->data = v;
    return *this;
}
DataVariant &DataVariant::operator=(const std::string &v) {
    this->data = v;
    return *this;
}
DataVariant &DataVariant::operator=(const float &v) {
    this->data = v;
    return *this;
}
DataVariant &DataVariant::operator=(const int &v) {
    this->data = v;
    return *this;
}
DataVariant &DataVariant::operator=(const bool &v) {
    this->data = v;
    return *this;
}