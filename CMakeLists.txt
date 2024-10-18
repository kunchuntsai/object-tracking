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
find_package(Protobuf REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(re2 REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Flatbuffers REQUIRED)

# Find ONNXRuntime
if(DEFINED ENV{ONNXRuntime_ROOT})
    set(ONNXRuntime_ROOT $ENV{ONNXRuntime_ROOT})
endif()
find_package(ONNXRuntime REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/processors)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ONNXRuntime_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${FLATBUFFERS_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

# Add all header files
file(GLOB HEADERS
    "src/*.h"
    "src/core/*.h"
    "src/processors/*.h"
    "src/utilities/*.h"
)

# Add all source files except main.cc
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
add_executable(object-tracking ${HEADERS} ${SOURCES} ${MAIN_SOURCE})

# Link libraries
target_link_libraries(object-tracking PRIVATE
    Threads::Threads
    ${OpenCV_LIBS}
    ${ONNXRuntime_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    nlohmann_json::nlohmann_json
    re2::re2
    Eigen3::Eigen
    ${FLATBUFFERS_LIBRARIES}
)

# Add CoreFoundation for macOS
if(APPLE)
    target_link_libraries(object-tracking PRIVATE ${CORE_FOUNDATION})
endif()

# Add test subdirectory
add_subdirectory(test)

# Export ONNXRuntime variables to subdirectories
set(ONNXRuntime_LIBRARIES ${ONNXRuntime_LIBRARIES} PARENT_SCOPE)
set(ONNXRuntime_INCLUDE_DIRS ${ONNXRuntime_INCLUDE_DIRS} PARENT_SCOPE)