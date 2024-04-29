include_directories(./include)
aux_source_directory(./src DIR_LIB_SRCS)
add_library(reactor SHARED ${DIR_LIB_SRCS})


add_executable(TestDenitrification ./test/TestDenitrification.cc)
target_link_libraries(TestDenitrification reactor io)