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
target_link_libraries(${PROJECT_NAME} PRIVATE OpenXLSX::OpenXLSX logger)

# 测试文件
add_executable(TestOpenXlsx test/TestOpenXlsx.cc)
target_link_libraries(TestOpenXlsx OpenXLSX::OpenXLSX)

add_executable(TestExcelIO test/TestExcelIO.cc)
target_link_libraries(TestExcelIO io)
target_include_directories(TestExcelIO PRIVATE include)
message("Leave ${PROJECT_SOURCE_DIR}")
