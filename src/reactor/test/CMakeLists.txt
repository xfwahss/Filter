cmake_minimum_required(VERSION 3.26)
project("Test-River")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# set( CMAKE_CXX_FLAGS "-std=c++11" )
set( CMAKE_BUILD_TYPE "Debug" )
include_directories(../include)
include_directories(../../io/include)
set (RiverIO
    ../../io/src/FileIO.cc
    ../../io/src/RiverFileIO.cc
)

# aux_source_directory(../../io/src IO)
aux_source_directory(../src REACTOR)

add_library(fileio SHARED   ${RiverIO})
add_library(reactor SHARED   ${REACTOR})
# add_executable(test ${TEST})
add_executable(TestRiver.o TestRiver.cc)
add_executable(TestReservoir.o TestReservoir.cc)
target_link_libraries(TestRiver.o fileio reactor)
target_link_libraries(TestReservoir.o fileio reactor)