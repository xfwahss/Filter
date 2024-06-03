cmake_minimum_required(VERSION 3.15)
project(utils)
message("Entring project ${PROJECT_NAME}")
set(CMAKE_CXX_STANDARD 17)

# 封装utils为动态库
aux_source_directory(src DIR_SRC)
add_library(${PROJECT_NAME} SHARED ${DIR_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include)


add_executable(TestUmath test/test_umath.cc)
target_link_libraries(TestUmath ${PROJECT_NAME})