cmake_minimum_required(VERSION 3.10)
project(LoggerONNXTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Use ONNX Runtime variables from parent scope
if(NOT ONNXRuntime_LIBRARIES)
    message(FATAL_ERROR "ONNXRuntime_LIBRARIES not set. Make sure ONNX Runtime is found in the parent CMakeLists.txt")
endif()

if(NOT ONNXRuntime_INCLUDE_DIRS)
    message(FATAL_ERROR "ONNXRuntime_INCLUDE_DIRS not set. Make sure ONNX Runtime is found in the parent CMakeLists.txt")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
    ${ONNXRuntime_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

# Add test files
set(TEST_SOURCES
    main_test.cc
    logger_test.cc
    onnx_test.cc
)

# Add ONNX model implementation
set(ONNX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core/onnx_model.cc
)

# Create the test executable
add_executable(run_tests ${TEST_SOURCES} ${ONNX_SOURCES})

# Link against ONNX Runtime and OpenCV
target_link_libraries(run_tests PRIVATE 
    ${ONNXRuntime_LIBRARIES}
    ${OpenCV_LIBS}
)

# Print debug information
message(STATUS "ONNXRuntime_INCLUDE_DIRS: ${ONNXRuntime_INCLUDE_DIRS}")
message(STATUS "ONNXRuntime_LIBRARIES: ${ONNXRuntime_LIBRARIES}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")

# Add a custom target to run the tests
add_custom_target(run_all_tests
    COMMAND run_tests
    DEPENDS run_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
    COMMENT "Running logger and ONNX tests"
)