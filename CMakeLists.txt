cmake_minimum_required(VERSION 3.10)

# Set the project name and version
if(DEFINED PROJECT_VERSION)
    project(ObjectTracking VERSION ${PROJECT_VERSION})
else()
    project(ObjectTracking VERSION 0.1.0)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/frames)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR})

# Add all header files
file(GLOB HEADERS
    "src/frames/*.h"
    "src/utilities/*.h"
)

# Add all source files (which are in the test directory)
file(GLOB SOURCES "test/*.cc")

# Find the threading library
find_package(Threads REQUIRED)

# Configure version header
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_BINARY_DIR}/version.h @ONLY)
else()
    file(WRITE ${CMAKE_BINARY_DIR}/version.h "#pragma once\n#define OBJECT_TRACKING_VERSION \"${PROJECT_VERSION}\"\n")
endif()

# Create the executable
add_executable(object-tracking ${HEADERS} ${SOURCES})

# Link the threading library and OpenCV
target_link_libraries(object-tracking PRIVATE Threads::Threads ${OpenCV_LIBS})