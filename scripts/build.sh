#!/bin/bash

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Default values
VERSION="0.1.0"
BUILD_TYPE="Release"
BUILD_DIR="build"
ONNX_VERSION="1.16.3"  # Match Docker version

# Get the directory of the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." &> /dev/null && pwd )"

# Function to print messages
log() {
    echo -e "${GREEN}[BUILD]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

# Function to detect environment
is_docker() {
    [ -f /.dockerenv ]
    return $?
}

# Function to check architecture
check_architecture() {
    local arch=$(uname -m)
    case $arch in
        arm64|aarch64)
            log "Detected ARM64 architecture (Apple Silicon)"
            export ARCH_FLAGS="-DCMAKE_OSX_ARCHITECTURES=arm64"
            ;;
        x86_64)
            log "Detected x86_64 architecture"
            export ARCH_FLAGS=""
            ;;
        *)
            error "Unsupported architecture: $arch"
            ;;
    esac
}

# Function to set up ONNX Runtime paths for macOS
setup_macos_paths() {
    if [[ "$OSTYPE" == "darwin"* ]] && ! is_docker; then
        if ! command -v brew &> /dev/null; then
            error "Homebrew is required for manual macOS build"
        fi
        
        HOMEBREW_PREFIX=$(brew --prefix)
        
        # Find ONNX Runtime in Homebrew
        if [ -d "${HOMEBREW_PREFIX}/Cellar/onnxruntime" ]; then
            INSTALLED_VERSION=$(brew list onnxruntime --versions | awk '{print $2}')
            log "Found ONNX Runtime version ${INSTALLED_VERSION}"
            
            if [ "$INSTALLED_VERSION" != "$ONNX_VERSION" ]; then
                warn "ONNX Runtime version mismatch: Docker uses $ONNX_VERSION, local version is $INSTALLED_VERSION"
            fi
            
            # Set up paths
            ONNX_INCLUDE_DIR="${HOMEBREW_PREFIX}/Cellar/onnxruntime/${INSTALLED_VERSION}/include"
            ONNX_LIBRARY="${HOMEBREW_PREFIX}/Cellar/onnxruntime/${INSTALLED_VERSION}/lib/libonnxruntime.dylib"
            
            # Create a simple CMake file for finding ONNX Runtime
            mkdir -p "${PROJECT_ROOT}/cmake"
            cat > "${PROJECT_ROOT}/cmake/FindONNXRuntime.cmake" << EOF
# FindONNXRuntime.cmake
# Custom finder for ONNXRuntime on macOS

set(ONNXRuntime_INCLUDE_DIRS "${ONNX_INCLUDE_DIR}")
set(ONNXRuntime_LIBRARIES "${ONNX_LIBRARY}")
set(ONNXRuntime_FOUND TRUE)

message(STATUS "ONNXRuntime_INCLUDE_DIRS: \${ONNXRuntime_INCLUDE_DIRS}")
message(STATUS "ONNXRuntime_LIBRARIES: \${ONNXRuntime_LIBRARIES}")
EOF
            
            log "Created custom FindONNXRuntime.cmake"
            log "Using ONNX Runtime from: ${HOMEBREW_PREFIX}/Cellar/onnxruntime/${INSTALLED_VERSION}"

            export ONNXRuntime_ROOT="$(brew --prefix)/opt/onnxruntime"
            log "Set ONNXRuntime_ROOT to ${ONNXRuntime_ROOT}"
            
            # Check for M4 specific optimizations
            if [[ $(sysctl -n machdep.cpu.brand_string) =~ "M4" ]]; then
                log "Detected M4 processor - enabling optimization flags"
                M4_FLAGS="-mcpu=apple-a14"
            else
                M4_FLAGS=""
            fi
        else
            error "ONNX Runtime not found. Please install it with: brew install onnxruntime"
        fi
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--version)
            VERSION="$2"
            shift 2
            ;;
        -t|--type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        *)
            error "Unknown option: $1"
            ;;
    esac
done

restore_logger() {
    # Placeholder for any cleanup needed
    log "Build process completed"
}

# Clean up from previous runs
trap restore_logger EXIT

# Check architecture and set up environment
check_architecture
setup_macos_paths

# Create or clean build directory
cd "$PROJECT_ROOT"

if [ -d "$BUILD_DIR" ]; then
    log "Cleaning existing build directory..."
    rm -rf $BUILD_DIR/*
else
    log "Creating build directory..."
    mkdir -p $BUILD_DIR
fi

# Navigate to build directory
cd $BUILD_DIR

# Configure CMake with proper paths
log "Configuring CMake..."
if [[ "$OSTYPE" == "darwin"* ]] && ! is_docker; then
    # macOS-specific CMake configuration
    cmake .. \
        -DPROJECT_VERSION=$VERSION \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_MODULE_PATH="${PROJECT_ROOT}/cmake" \
        -DCMAKE_PREFIX_PATH="${HOMEBREW_PREFIX}" \
        -DCMAKE_CXX_FLAGS="${M4_FLAGS}" \
        -DCMAKE_MACOSX_RPATH=ON \
        ${ARCH_FLAGS}
else
    # Default CMake configuration for Docker
    cmake .. \
        -DPROJECT_VERSION=$VERSION \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi

# Build the project
log "Building project..."
if command -v nproc &> /dev/null; then
    make -j$(nproc)
elif command -v sysctl &> /dev/null; then
    make -j$(sysctl -n hw.ncpu)
else
    make -j4  # Default to 4 cores if can't detect
fi

# Create necessary directories
log "Creating output directories..."
mkdir -p ../output ../config

# Print completion message
log "Build complete!"
echo "Build details:"
echo "  Version: $VERSION"
echo "  Build type: $BUILD_TYPE"
echo "  Build directory: ${PROJECT_ROOT}/${BUILD_DIR}"
if [[ "$OSTYPE" == "darwin"* ]] && ! is_docker; then
    echo "  ONNX Runtime version: $INSTALLED_VERSION"
    echo "  ONNX Runtime include: ${ONNX_INCLUDE_DIR}"
    echo "  ONNX Runtime library: ${ONNX_LIBRARY}"
    if [[ $(sysctl -n machdep.cpu.brand_string) =~ "M4" ]]; then
        echo "  M4 optimizations: enabled"
    fi
fi

# Verify the build
if [ -f "object-tracking" ]; then
    log "Build verification: OK"
else
    error "Build verification failed: executable not found"
fi