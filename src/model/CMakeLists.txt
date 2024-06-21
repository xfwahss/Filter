project("model")
message("Entering project model")

add_executable(Model_Validate src/Validate.cc)
target_link_libraries(Model_Validate io filter utils)


add_executable(velocity_tracker src/velocity_tracker.cc)
target_link_libraries(velocity_tracker io filter utils)


add_executable(ModelIntegrate src/ModelIntegrate.cc)
target_link_libraries(ModelIntegrate io filter reactor utils)

add_executable(ModelReservoir src/ModelReservoir.cc)
target_link_libraries(ModelReservoir io filter reactor)

add_executable(ModelDegrade src/Model_Degrade.cc)
target_link_libraries(ModelDegrade io filter reactor utils)

add_executable(TestSpdLog src/Test_Logger.cc)
target_link_libraries(TestSpdLog spdlog)