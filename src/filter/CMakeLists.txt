include_directories(./include)
aux_source_directory(./src DIR_LIB_SRCS)
add_library(filter SHARED ${DIR_LIB_SRCS})



add_executable(TestKalmanFilter test/TestKalmanFilter.cc)
add_executable(TestKalmanFilterWithFileIO test/TestKalmanFilterWithFileIO.cc)
# add_executable(TestEnsembleKalmanFilter test/TestEnsembleKalman.cc)
add_executable(TestEnsembleSin test/TestEnsembleSin.cc)

target_link_libraries(TestKalmanFilter filter io)
target_link_libraries(TestKalmanFilterWithFileIO filter io)
# target_link_libraries(TestEnsembleKalmanFilter filter io)
target_link_libraries(TestEnsembleSin filter io)