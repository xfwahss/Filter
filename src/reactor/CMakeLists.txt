include_directories(./include)
aux_source_directory(./src DIR_LIB_SRCS)
add_library(reactor SHARED ${DIR_LIB_SRCS})


add_executable(TestDenitrification ./test/TestDenitrification.cc)
add_executable(TestNitrification ./test/testNitrification.cc)
add_executable(TestRiverSystem ./test/TestRiverSystem.cc)
target_link_libraries(TestDenitrification reactor io)
target_link_libraries(TestNitrification reactor io)
target_link_libraries(TestRiverSystem reactor io)