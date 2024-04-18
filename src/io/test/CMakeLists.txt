cmake_minimum_required(VERSION 3.26)
project("testio")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set( CMAKE_BUILD_TYPE "Debug" )

include_directories(../../third_party/eigen)
include_directories(../include)

set(IOCC 
../src/FileIO.cc
../src/FilterIO.cc
)

add_library(fileio SHARED ${IOCC})
add_executable(TestFilterIO TestFilterIO.cc)
target_link_libraries(TestFilterIO fileio)
