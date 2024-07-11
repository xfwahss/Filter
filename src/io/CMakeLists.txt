project(io)
message("Enter ${PROJECT_SOURCE_DIR}")
set(OPENXLSX_LIBRARY_TYPE SHARED)
set(XLSXDIR ${CMAKE_SOURCE_DIR}/src/third_party/openxlsx/OpenXLSX)
add_subdirectory(${XLSXDIR} ${CMAKE_BINARY_DIR}/openxlsx)

# 封装动态链接库
set(LOGGERDIR ${CMAKE_SOURCE_DIR}/src/logger/include)
aux_source_directory(src LIBSRC)
add_library(${PROJECT_NAME} SHARED ${LIBSRC})
target_include_directories(${PROJECT_NAME} PUBLIC include ${XLSXDIR} ${LOGGERDIR} ${CMAKE_BINARY_DIR}/openxlsx)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenXLSX::OpenXLSX logger)

# 测试文件
if(${BUILD_TEST})
aux_source_directory(test TEST_SRC)
foreach(src_file ${TEST_SRC})
    get_filename_component(test_exe_name ${src_file} NAME_WE)
    add_executable(${test_exe_name} ${src_file})
    target_include_directories(${test_exe_name} PRIVATE include)
    target_link_libraries(${test_exe_name} PRIVATE ${PROJECT_NAME})
    add_test(NAME ${test_exe_name}
             COMMAND ${test_exe_name}
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach(src_file ${TEST_SRC})
endif()
message("Leave ${PROJECT_SOURCE_DIR}")
