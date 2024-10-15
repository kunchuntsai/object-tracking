# FindONNXRuntime.cmake

# Find the ONNX Runtime library
#
# The following variables are optionally searched for defaults
#  ONNXRuntime_ROOT: Base directory where ONNX Runtime is found
#
# The following are set after configuration is done:
#  ONNXRuntime_FOUND
#  ONNXRuntime_INCLUDE_DIRS
#  ONNXRuntime_LIBRARIES

find_path(ONNXRuntime_INCLUDE_DIR
  NAMES onnxruntime_cxx_api.h
  PATHS
    ${ONNXRuntime_ROOT}/include
    $ENV{ONNXRuntime_ROOT}/include
)

find_library(ONNXRuntime_LIBRARY
  NAMES onnxruntime
  PATHS
    ${ONNXRuntime_ROOT}/lib
    $ENV{ONNXRuntime_ROOT}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ONNXRuntime
  FOUND_VAR ONNXRuntime_FOUND
  REQUIRED_VARS
    ONNXRuntime_LIBRARY
    ONNXRuntime_INCLUDE_DIR
)

if(ONNXRuntime_FOUND)
  set(ONNXRuntime_LIBRARIES ${ONNXRuntime_LIBRARY})
  set(ONNXRuntime_INCLUDE_DIRS ${ONNXRuntime_INCLUDE_DIR})
endif()

mark_as_advanced(ONNXRuntime_INCLUDE_DIR ONNXRuntime_LIBRARY)