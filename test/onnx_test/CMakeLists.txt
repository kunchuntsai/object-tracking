cmake_minimum_required(VERSION 3.10)
project(ONNXRuntimeTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use ONNX Runtime variables from parent scope
if(NOT ONNXRuntime_LIBRARIES)
    message(FATAL_ERROR "ONNXRuntime_LIBRARIES not set. Make sure ONNX Runtime is found in the parent CMakeLists.txt")
endif()

if(NOT ONNXRuntime_INCLUDE_DIRS)
    message(FATAL_ERROR "ONNXRuntime_INCLUDE_DIRS not set. Make sure ONNX Runtime is found in the parent CMakeLists.txt")
endif()

# Add the executable
add_executable(test_onnx test_onnx.cc)

# Include ONNX Runtime headers
target_include_directories(test_onnx PRIVATE ${ONNXRuntime_INCLUDE_DIRS})

# Link against ONNX Runtime
target_link_libraries(test_onnx PRIVATE ${ONNXRuntime_LIBRARIES})

# Print debug information
message(STATUS "ONNXRuntime_INCLUDE_DIRS: ${ONNXRuntime_INCLUDE_DIRS}")
message(STATUS "ONNXRuntime_LIBRARIES: ${ONNXRuntime_LIBRARIES}")

# Add a custom target to run the test with a model
add_custom_target(run_onnx_test
    COMMAND test_onnx ${CMAKE_CURRENT_SOURCE_DIR}/model.onnx
    DEPENDS test_onnx
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running ONNX Runtime test with model.onnx"
)