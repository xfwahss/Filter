#ifndef DATAVARIANT_H
#define DATAVARIANT_H
#include <string>
#include <variant>

class DataVariant {
  private:
    std::variant<std::string, double, float, int, bool> data;

  public:
    DataVariant()  = default;
    ~DataVariant() = default;
    // 转化返回double类型
    double dvalue();
    // 转化返回为float 类型
    float fvalue();
    // 转化返回int类型
    int ivalue();
    // 转化返回string类型
    std::string svalue();
    // 转化返回bool类型
    bool bvalue();
    DataVariant &operator=(const double &v);
    DataVariant &operator=(const std::string &v);
    DataVariant &operator=(const float &v);
    DataVariant &operator=(const int &v);
    DataVariant &operator=(const bool &v);
};
#endif