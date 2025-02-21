cmake_minimum_required(VERSION 3.10)

# Set the project name and version
if(DEFINED PROJECT_VERSION)
    project(ObjectTracking VERSION ${PROJECT_VERSION})
else()
    project(ObjectTracking VERSION 0.1.0)
endif()

# Set CMP0074 policy to NEW
cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the custom CMake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find required packages
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

# Find ONNXRuntime
if(DEFINED ENV{ONNXRuntime_ROOT})
    set(ONNXRuntime_ROOT $ENV{ONNXRuntime_ROOT})
endif()
find_package(ONNXRuntime REQUIRED)

# Extract ONNX Runtime version from the library path
string(REGEX MATCH "onnxruntime-([0-9]+\.[0-9]+\.[0-9]+)" ONNXRUNTIME_VERSION ${ONNXRuntime_LIBRARIES})
set(ONNXRUNTIME_VERSION ${CMAKE_MATCH_1})

# Check for CoreML support in ONNX Runtime
set(COREML_AVAILABLE FALSE)
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_COREML")

    # Check if ONNXRuntime is built with CoreML support
    if(EXISTS "${ONNXRuntime_INCLUDE_DIRS}/onnxruntime_coreml_provider_factory.h")
        message(STATUS "ONNX Runtime with CoreML support detected")
        add_definitions(-DONNXRUNTIME_COREML_AVAILABLE)
        set(COREML_AVAILABLE TRUE)
    else()
        message(WARNING "ONNX Runtime found, but CoreML support is not available.")
        message(STATUS "To enable CoreML support, you may need to rebuild ONNX Runtime with CoreML enabled.")
    endif()

    # Additional CoreML checks
    if(COREML_AVAILABLE)
        # Check for specific CoreML headers
        if(EXISTS "${ONNXRuntime_INCLUDE_DIRS}/core/providers/coreml/coreml_provider_factory.h")
            message(STATUS "CoreML provider factory header found")
        else()
            message(WARNING "CoreML provider factory header not found")
        endif()

        # Check for CoreML library
        find_library(COREML_LIBRARY CoreML)
        if(COREML_LIBRARY)
            message(STATUS "CoreML library found: ${COREML_LIBRARY}")
            target_link_libraries(object-tracking PRIVATE ${COREML_LIBRARY})
        else()
            message(WARNING "CoreML library not found")
        endif()

        # Add CoreML-specific compile definitions
        add_compile_definitions(USE_COREML)
        add_compile_definitions(ONNXRUNTIME_COREML_AVAILABLE)
    endif()
else()
    message(STATUS "CoreML support not available (not on Apple platform)")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/processors)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ONNXRuntime_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

# Add all header files
file(GLOB HEADERS
    "src/*.h"
    "src/core/*.h"
    "src/processors/*.h"
    "src/utilities/*.h"
)

# Add all source files including main.cc
file(GLOB SOURCES
    "src/*.cc"
    "src/core/*.cc"
    "src/processors/*.cc"
    "src/utilities/*.cc"
)

# Find the threading library
find_package(Threads REQUIRED)

# Add CoreFoundation framework for macOS
if(APPLE)
    find_library(CORE_FOUNDATION CoreFoundation)
endif()

# Configure version header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)

# Create the main executable
add_executable(object-tracking ${HEADERS} ${SOURCES})

# Link libraries
target_link_libraries(object-tracking PRIVATE
    Threads::Threads
    ${OpenCV_LIBS}
    ${ONNXRuntime_LIBRARIES}
    Eigen3::Eigen
)

# Add CoreFoundation for macOS
if(APPLE)
    target_link_libraries(object-tracking PRIVATE ${CORE_FOUNDATION})
endif()

# Add test subdirectory
add_subdirectory(test)

# Print ONNX Runtime configuration
message(STATUS "ONNX Runtime configuration:")
message(STATUS "  Version: ${ONNXRUNTIME_VERSION}")
message(STATUS "  Include Dirs: ${ONNXRuntime_INCLUDE_DIRS}")
message(STATUS "  Libraries: ${ONNXRuntime_LIBRARIES}")

if(APPLE)
    message(STATUS "CoreML Configuration:")
    message(STATUS "  COREML_AVAILABLE: ${COREML_AVAILABLE}")
    if(COREML_AVAILABLE)
        message(STATUS "  COREML_LIBRARY: ${COREML_LIBRARY}")
    endif()
endif()