project(filter)
message("Entering project ${PROJECT_NAME}")

# include_directories(./include)
aux_source_directory(src DIR_LIB_SRCS)
add_library(${PROJECT_NAME} SHARED ${DIR_LIB_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PRIVATE io)



# add_executable(TestKalmanFilter test/TestKalmanFilter.cc)
# add_executable(TestKalmanFilterWithFileIO test/TestKalmanFilterWithFileIO.cc)
# add_executable(TestEnsembleKalmanFilter test/TestEnsembleKalman.cc)
add_executable(TestEnsembleSin test/TestEnsembleSin.cc)

# target_link_libraries(TestKalmanFilter filter io)
# target_link_libraries(TestKalmanFilterWithFileIO filter io)
# target_link_libraries(TestEnsembleKalmanFilter filter io)
target_link_libraries(TestEnsembleSin filter io)