project(model)
message(STATUS "Enter ${PROJECT_SOURCE_DIR}")

# 模型编译
set(IOHEADER ${CMAKE_SOURCE_DIR}/src/io/include)
set(FILTERHEADER ${CMAKE_SOURCE_DIR}/src/filter/include)
set(UTILSHEADER ${CMAKE_SOURCE_DIR}/src/utils/include)
set(REACTORHEADER ${CMAKE_SOURCE_DIR}/src/reactor/include)
set(LOGGERHEADER ${CMAKE_SOURCE_DIR}/src/logger/include)
aux_source_directory(src MODELSRC)
foreach(model_file ${MODELSRC})
    get_filename_component(model_exe_name ${model_file} NAME_WE)
    add_executable(${model_exe_name} ${model_file})
    target_include_directories(${model_exe_name} PRIVATE {
        ${IOHEADER}
        ${FILTERHEADER}
        ${UTILSHEADER}
        ${REACTOR_HEADER}
        ${LOGGERHEADER}
        ${ROOT_CONFIG_DIR}
    })
    target_link_libraries(${model_exe_name} PRIVATE io filter utils reactor logger)
endforeach()


# 添加测试
if(${BUILD_TEST})
aux_source_directory(test TESTSRC)
foreach(test_file ${TESTSRC})
    get_filename_component(test_exe_name ${test_file} NAME_WE)
    add_executable(${test_exe_name} ${test_file})
    target_include_directories(${test_exe_name} PRIVATE include)

    target_include_directories(${test_exe_name} PRIVATE {
        ${IOHEADER}
        ${FILTERHEADER}
        ${UTILSHEADER}
        ${REACTOR_HEADER}
        ${LOGGERHEADER}
    })
    target_link_libraries(${test_exe_name} PRIVATE io filter utils reactor logger)
    add_test(NAME ${test_exe_name}
             COMMAND ${test_exe_name}
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
endif()
message(STATUS "Leave ${PROJECT_SOURCE_DIR}")
