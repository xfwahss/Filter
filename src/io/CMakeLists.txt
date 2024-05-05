include_directories(./include)
aux_source_directory(./src DIR_LIB_SRCS)
add_library(io SHARED ${DIR_LIB_SRCS})

add_executable(TestOpenXlsx test/TestOpenXlsx.cc)
target_link_libraries(TestOpenXlsx OpenXLSX::OpenXLSX)