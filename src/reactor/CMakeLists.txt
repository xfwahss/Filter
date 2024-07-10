project(reactor)
message("Enter ${PROJECT_SOURCE_DIR}")

# 添加动态链接库
aux_source_directory(src LIBSRC)
add_library(${PROJECT_NAME} SHARED ${LIBSRC})
target_include_directories(${PROJECT_NAME} PUBLIC include)

# 添加测试
if(${BUILD_TEST})
aux_source_directory(test TESTSRC)
foreach(test_file ${TESTSRC})
    get_filename_component(test_exe_name ${test_file} NAME_WE)
    add_executable(${test_exe_name} ${test_file})
    target_include_directories(${test_exe_name} PRIVATE include)
    target_link_libraries(${test_exe_name} PRIVATE ${PROJECT_NAME})
    add_test(NAME ${test_exe_name}
             COMMAND ${test_exe_name}
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
endif()
message("Leave ${PROJECT_SOURCE_DIR}")