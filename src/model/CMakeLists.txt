project("model")
message("Entering project model")

add_executable(Model_Validate src/Validate.cc)
target_link_libraries(Model_Validate io filter)
