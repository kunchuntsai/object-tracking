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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/processors)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_BINARY_DIR})

# Add all header files
file(GLOB HEADERS
    "src/*.h"
    "src/core/*.h"
    "src/processors/*.h"
    "src/utilities/*.h"
)

# Add all source files
file(GLOB SOURCES
    "src/*.cc"
    "src/core/*.cc"
    "src/processors/*.cc"
)

# Add test files
file(GLOB TEST_SOURCES "test/*.cc")

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

# Link the threading library, OpenCV, and CoreFoundation (if on macOS)
if(APPLE)
    target_link_libraries(object-tracking PRIVATE Threads::Threads ${OpenCV_LIBS} ${CORE_FOUNDATION})
else()
    target_link_libraries(object-tracking PRIVATE Threads::Threads ${OpenCV_LIBS})
endif()

# Create test executables
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source} ${HEADERS} ${SOURCES})
    if(APPLE)
        target_link_libraries(${test_name} PRIVATE Threads::Threads ${OpenCV_LIBS} ${CORE_FOUNDATION})
    else()
        target_link_libraries(${test_name} PRIVATE Threads::Threads ${OpenCV_LIBS})
    endif()
endforeach()