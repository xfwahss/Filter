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

include_directories(src/third_party/eigen)
add_subdirectory(src/io)
add_subdirectory(src/filter)
add_subdirectory(src/reactor)
add_subdirectory(src/model)



