project(utils)
message(STATUS "Enter ${PROJECT_SOURCE_DIR}")

# 封装utils为动态库
aux_source_directory(${PROJECT_SOURCE_DIR}/src LIBSRC)
add_library(${PROJECT_NAME} SHARED ${LIBSRC})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE logger)


# 测试文件
if(${BUILD_TEST})
aux_source_directory(test TEST_SRC)
foreach(src_file ${TEST_SRC})
    get_filename_component(test_exe_name ${src_file} NAME_WE)
    add_executable(${test_exe_name} ${src_file})
    target_include_directories(${test_exe_name} PRIVATE include)
    target_link_libraries(${test_exe_name} PRIVATE ${PROJECT_NAME})
    target_link_libraries(${test_exe_name} PRIVATE logger)
    add_test(NAME ${test_exe_name}
             COMMAND ${test_exe_name}
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach(src_file ${TEST_SRC})
endif()
message(STATUS "Leave ${PROJECT_SOURCE_DIR}")