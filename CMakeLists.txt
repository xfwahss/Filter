cmake_minimum_required(VERSION 3.26)
project("Filter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成compile_commands.json文件
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set( CMAKE_BUILD_TYPE "Debug" )
include_directories(src/third_party/eigen)

add_subdirectory(src/third_party/openxlsx/OpenXLSX)
add_subdirectory(src/io)
add_subdirectory(src/filter)
add_subdirectory(src/reactor)


# Set the build output location to a common directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

