# FindONNXRuntime.cmake
# Unified finder for ONNXRuntime that works across macOS (including M4) and Docker/Nix environments

# The following variables are optionally searched for defaults
#  ONNXRuntime_ROOT: Base directory where ONNX Runtime is found
#
# The following are set after configuration is done:
#  ONNXRuntime_FOUND
#  ONNXRuntime_INCLUDE_DIRS
#  ONNXRuntime_LIBRARIES

# Detect if we're running on macOS
if(APPLE)
  # Check if we're using Homebrew
  find_program(BREW_EXECUTABLE brew)
  if(BREW_EXECUTABLE)
    execute_process(
      COMMAND ${BREW_EXECUTABLE} --prefix
      OUTPUT_VARIABLE HOMEBREW_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Try to find installed onnxruntime via Homebrew
    set(BREW_ONNX_DIR "${HOMEBREW_PREFIX}/Cellar/onnxruntime")
    if(EXISTS ${BREW_ONNX_DIR})
      # Find the most recent version if multiple are installed
      file(GLOB ONNX_VERSIONS LIST_DIRECTORIES true "${BREW_ONNX_DIR}/*")
      list(SORT ONNX_VERSIONS)
      list(REVERSE ONNX_VERSIONS)
      list(GET ONNX_VERSIONS 0 LATEST_ONNX_VERSION_DIR)

      get_filename_component(ONNX_VERSION_NAME ${LATEST_ONNX_VERSION_DIR} NAME)
      message(STATUS "Found Homebrew ONNXRuntime version: ${ONNX_VERSION_NAME}")

      # Set up hardcoded paths for macOS/Homebrew
      set(ONNXRuntime_INCLUDE_DIRS "${LATEST_ONNX_VERSION_DIR}/include")
      set(ONNXRuntime_LIBRARIES "${LATEST_ONNX_VERSION_DIR}/lib/libonnxruntime.dylib")
      set(ONNXRuntime_FOUND TRUE)

      # Early return for macOS Homebrew case
      message(STATUS "Using Homebrew ONNXRuntime installation")
      message(STATUS "ONNXRuntime_INCLUDE_DIRS: ${ONNXRuntime_INCLUDE_DIRS}")
      message(STATUS "ONNXRuntime_LIBRARIES: ${ONNXRuntime_LIBRARIES}")
      return()
    endif()
  endif()
endif()

# If we're not on macOS or Homebrew isn't found, use the standard CMake find process
# Check for explicitly set paths from ONNXRuntime_ROOT variable
if(DEFINED ONNXRuntime_ROOT OR DEFINED ENV{ONNXRuntime_ROOT})
  set(ONNXRuntime_ROOT_DIR ${ONNXRuntime_ROOT} $ENV{ONNXRuntime_ROOT})
  message(STATUS "Using provided ONNXRuntime_ROOT: ${ONNXRuntime_ROOT_DIR}")
endif()

# Standard find process
find_path(ONNXRuntime_INCLUDE_DIR
  NAMES onnxruntime_cxx_api.h
  PATHS
    ${ONNXRuntime_ROOT_DIR}/include
    /usr/local/include
    /usr/include
    /opt/onnxruntime/include
  DOC "ONNX Runtime include directory"
)

find_library(ONNXRuntime_LIBRARY
  NAMES onnxruntime
  PATHS
    ${ONNXRuntime_ROOT_DIR}/lib
    /usr/local/lib
    /usr/lib
    /usr/lib/x86_64-linux-gnu
    /opt/onnxruntime/lib
  DOC "ONNX Runtime library"
)

# Use standard FindPackage handling to set found status
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ONNXRuntime
  FOUND_VAR ONNXRuntime_FOUND
  REQUIRED_VARS ONNXRuntime_LIBRARY ONNXRuntime_INCLUDE_DIR
)

# Set final variables
if(ONNXRuntime_FOUND)
  set(ONNXRuntime_LIBRARIES ${ONNXRuntime_LIBRARY})
  set(ONNXRuntime_INCLUDE_DIRS ${ONNXRuntime_INCLUDE_DIR})
  message(STATUS "ONNXRuntime_INCLUDE_DIRS: ${ONNXRuntime_INCLUDE_DIRS}")
  message(STATUS "ONNXRuntime_LIBRARIES: ${ONNXRuntime_LIBRARIES}")
endif()

mark_as_advanced(ONNXRuntime_INCLUDE_DIR ONNXRuntime_LIBRARY)