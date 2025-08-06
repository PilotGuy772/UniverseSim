cmake_minimum_required(VERSION 3.14)

# Create a test executable target from all test source files
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Define test executable
add_executable(UniverseSimTests ${TEST_SOURCES}
        Core.test.cpp
        GPU.test.cpp
        EntityManagement.test.cpp
        SimulationLoop/Simulation.test.cpp
        SimulationLoop/Inputs.test.cpp
        SimulationLoop/EntityControlAndBufferManagement.test.cpp)

# Fetch Catch2
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.9.0
)
FetchContent_MakeAvailable(Catch2)

# Link Catch2 and your project library
include_directories(UniverseSimTests PRIVATE ${Catch2_SOURCE_DIR}/include)

# Link main project library
target_include_directories(UniverseSimTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(UniverseSimTests
        PUBLIC
        Catch2::Catch2WithMain
        UniverseSim_lib
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Make sure the Catch2 test integration in CLion picks this up
#catch_discover_tests(UniverseSim2Tests)
