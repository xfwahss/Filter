cmake_minimum_required(VERSION 3.26)
project("Filter")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 生成compile_commands.json文件
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set( CMAKE_BUILD_TYPE "Debug" )
# 归档目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# 全局配置文件
configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
set(ROOT_CONFIG_DIR ${PROJECT_BINARY_DIR})
# 全局允许测试，子目录中详细展开
option(BUILD_TEST "Whether or not to build the tests" ON)
if (${BUILD_TEST})
    enable_testing()  # Enable testing only works in root scope
endif ()

# 全局包括eigen的头文件
include_directories(src/third_party/eigen)
# 全局包括indicators的头文件，进度条库
include_directories(src/third_party/indicators/include)
# 全局添加logger库
add_subdirectory(src/logger ${CMAKE_BINARY_DIR}/logger)
include_directories(${CMAKE_SOURCE_DIR}/src/logger/include)

# 全局添加args库
include_directories(src/third_party/args ${CMAKE_BINARY_DIR}/args)
# 编译utils库
add_subdirectory(src/utils ${CMAKE_BINARY_DIR}/utils)
# 编译io库
add_subdirectory(src/io ${CMAKE_BINARY_DIR}/io)
# 编译reactor库
add_subdirectory(src/reactor ${CMAKE_BINARY_DIR}/reactor)
# 编译filter库
add_subdirectory(src/filter ${CMAKE_BINARY_DIR}/filter)
# 编译modle库
add_subdirectory(src/model ${CMAKE_BINARY_DIR}/model)


# 测试文件
if(${BUILD_TEST})
aux_source_directory(test/src TEST_SRC)
foreach(src_file ${TEST_SRC})
    get_filename_component(test_exe_name ${src_file} NAME_WE)
    add_executable(${test_exe_name} ${src_file})
    add_test(NAME ${test_exe_name}
             COMMAND ${test_exe_name}
             WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach(src_file ${TEST_SRC})
endif()



