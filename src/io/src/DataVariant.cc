/*
 * @Description: DataVariant.cc
 * @version: v0.1
 * @Author: xfwahss
 * @Date: 2024-07-11 18:22:19
 * @LastEditors: xfwahss
 * @LastEditTime: 2024-07-12 09:35:53
 */
#include <DataVariant.h>
#include <cctype>
#include <logger.h>


double DataVariant::stod(const std::string &s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            logger::get("DataVariant-cc")->warn("Can't covert {} to double, replaced with -999.0", s);
            return -999.0;
        }
    }
    return std::stoi(s);
}

float DataVariant::stof(const std::string &s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            logger::get("DataVariant-cc")->warn("Can't covert {} to float, replaced with -999.0", s);
            return -999.0;
        }
    }
    return std::stof(s);
}

int DataVariant::stoi(const std::string &s) {
    for (char c : s) {
        if (!std::isdigit(c)) {
            logger::get("DataVariant-cc")->warn("Can't covert {} to int, replaced with -999.0", s);
            return -999.0;
        }
    }
    return std::stoi(s);
}

bool DataVariant::stob(const std::string &s) {
    std::string lower_str = s;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if(lower_str == "true"){
        return true;
    } else if(lower_str == "false"){
        return false;
    } else{
        logger::get("DataVariant-cc")->warn("Can't covert {} to bool, replaced with false", s);
        return false;
    }
}

double DataVariant::dvalue() {
    int index = data.index();
    switch (index) {
    case 0:
        return DataVariant::stod(std::get<std::string>(data));
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
        return DataVariant::stof(std::get<std::string>(data));
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
        return DataVariant::stoi(std::get<std::string>(data));
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
        return DataVariant::stob(std::get<std::string>(data));
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