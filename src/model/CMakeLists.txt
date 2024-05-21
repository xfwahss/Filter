project("model")
message("Entering project model")

add_executable(Model_Validate src/Validate.cc)
target_link_libraries(Model_Validate io filter)


add_executable(velocity_tracker src/velocity_tracker.cc)
target_link_libraries(velocity_tracker io filter)


add_executable(ModelIntegrate src/ModelIntegrate.cc)
target_link_libraries(ModelIntegrate io filter reactor)
