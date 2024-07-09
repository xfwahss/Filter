project("model")
message(STATUS "Entering project model")

add_executable(Model_Validate src/Validate.cc)
target_link_libraries(Model_Validate io filter utils)


add_executable(velocity_tracker src/velocity_tracker.cc)
target_link_libraries(velocity_tracker io filter utils)


add_executable(velocity_tracker_1d src/velocity_tracker_1d.cc)
target_link_libraries(velocity_tracker_1d io filter utils)


add_executable(ModelIntegrate src/ModelIntegrate.cc)
target_link_libraries(ModelIntegrate io filter reactor utils)

add_executable(ModelReservoir src/ModelReservoir.cc)
target_link_libraries(ModelReservoir io filter reactor)

add_executable(ModelDegrade src/Model_Degrade.cc)
target_link_libraries(ModelDegrade io filter reactor utils spdlog)
target_include_directories(ModelDegrade PRIVATE ${ROOT_CONFIG_DIR})


add_executable(TestArgs test/TestArgs.cc)

add_executable(Model2dDegrade ${PROJECT_SOURCE_DIR}/src/Model2dDegrade.cc)
target_include_directories(Model2dDegrade PRIVATE ${ROOT_CONFIG_DIR} ${CMAKE_SOURCE_DIR}/src/filter/include)
target_link_libraries(Model2dDegrade filter utils spdlog)
