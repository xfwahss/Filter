cmake_minimum_required(VERSION 3.26)
project("main")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# set( CMAKE_CXX_FLAGS "-std=c++11" )
set( CMAKE_BUILD_TYPE "Debug" )
include_directories(../reactor/include)
include_directories(../io/include)
set (IO
    ../io/src/FileIO.cc
    ../io/src/RiverFileIO.cc
    ../io/src/ReservoirFileIO.cc
)

# aux_source_directory(../../io/src IO)
aux_source_directory(../reactor/src REACTOR)

add_library(fileio SHARED   ${IO})
add_library(reactor SHARED   ${REACTOR})
# add_executable(test ${TEST})
# add_executable(TestRiver.o TestRiver.cc)
# add_executable(TestReservoir.o TestReservoir.cc)
add_executable(main main.cc)
target_link_libraries(main fileio reactor)