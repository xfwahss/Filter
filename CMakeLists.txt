cmake_minimum_required(VERSION 3.26)
project("Filter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成compile_commands.json文件
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set( CMAKE_BUILD_TYPE "Debug" )
# 归档目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 全局配置文件
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
set(ROOT_CONFIG_DIR ${PROJECT_BINARY_DIR})
# 全局包括eigen的头文件
include_directories(src/third_party/eigen)
# 编译spdlog动态库，加快编译速度
add_subdirectory(src/third_party/spdlog)
include_directories(src/third_party/spdlog/include)
# 全局添加args库
include_directories(src/third_party/args)
# 编译utils库
add_subdirectory(src/utils)
# 编译io库
add_subdirectory(src/io)
# 编译reactor库
add_subdirectory(src/reactor)
# 编译filter库
add_subdirectory(src/filter)
# 编译modle库
add_subdirectory(src/model)



