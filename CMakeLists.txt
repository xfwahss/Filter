cmake_minimum_required(VERSION 3.26)
project("Filter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成compile_commands.json文件
set(CMAKE_CXX_STANDARD 17)
# set(ENV{CXX} g++)
# set(ENV{CC} gcc)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set( CMAKE_BUILD_TYPE "Debug" )
# 归档目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 配置文件
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
set(ROOT_CONFIG_DIR ${PROJECT_BINARY_DIR})

include_directories(src/third_party/eigen)
# 也是只用头文件就行
add_subdirectory(src/third_party/spdlog)
include_directories(src/third_party/spdlog/include)

add_subdirectory(src/utils)
add_subdirectory(src/io)
add_subdirectory(src/reactor)
add_subdirectory(src/filter)
add_subdirectory(src/model)



