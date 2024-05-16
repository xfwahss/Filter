cmake_minimum_required(VERSION 3.15)
project(io)
message("Entring project ${PROJECT_NAME}")
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(../third_party/openxlsx/OpenXLSX ${CMAKE_BINARY_DIR}/src/third_party/openxlsx)

# 封装Project为动态库
aux_source_directory(src DIR_SRC)
add_library(${PROJECT_NAME} SHARED ${DIR_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenXLSX::OpenXLSX)

# 测试文件
add_executable(TestOpenXlsx test/TestOpenXlsx.cc)
target_link_libraries(TestOpenXlsx OpenXLSX::OpenXLSX)

add_executable(TestExcelIO test/TestExcelIO.cc)
target_link_libraries(TestExcelIO io)

add_executable(TestFilterIO test/TestFilterIO.cc)
target_link_libraries(TestFilterIO io)


add_executable(TestMatFileIO test/TestMatrixIO.cc)
target_link_libraries(TestMatFileIO io)

add_executable(TestRiverFileIO test/TestRiverFileIO.cc)
target_link_libraries(TestRiverFileIO io)