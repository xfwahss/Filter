# Filter
集合卡尔曼滤波模板类，0维水质模型基本组件、Excel文件读写
## 系统要求
编写环境Centos Stream9，系统需要安装git, cmake, make, gcc, gcc-g++, python3及第三方包:matplotlib, pandas.

### 源码下载命令
```bash
git clone --recursive https://gitee.com/xfwahss/Filter.git
```
### 代码编译(Windows)
```bash
cd Filter/build
cmake -G "MinGW Makefiles"  -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_MAKE_PROGRAM=make .. # windows
make
```

## 集合卡尔曼滤波测试
*测试文件编译及运行*
```bash
cd ./test/scripts
python3 test_ensemble_with_sin.py
```