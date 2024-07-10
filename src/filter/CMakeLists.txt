project(filter)
message("Enter ${PROJECT_SOURCE_DIR}")

# 创建动态链接库
aux_source_directory(src LIBSRC)
add_library(${PROJECT_NAME} SHARED ${LIBSRC})
target_include_directories(${PROJECT_NAME} PUBLIC 
                           ${PROJECT_SOURCE_DIR}/include 
                           ${CMAKE_SOURCE_DIR}/src/io/include
                           ${CMAKE_SOURCE_DIR}/src/utils/include)
target_link_libraries(${PROJECT_NAME} PRIVATE utils io logger)


# 测试文件
# add_executable(TestKalmanFilter test/TestKalmanFilter.cc)
# add_executable(TestKalmanFilterWithFileIO test/TestKalmanFilterWithFileIO.cc)
# add_executable(TestEnsembleKalmanFilter test/TestEnsembleKalman.cc)
# add_executable(TestEnsembleSin test/TestEnsembleSin.cc)

# target_link_libraries(TestKalmanFilter filter io)
# target_link_libraries(TestKalmanFilterWithFileIO filter io)
# target_link_libraries(TestEnsembleKalmanFilter filter io)
# target_link_libraries(TestEnsembleSin filter io utils spdlog)
message("Leave ${PROJECT_SOURCE_DIR}")