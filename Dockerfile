# Dockerfile - Environment Setup Only
FROM ubuntu:24.04

# Prevent timezone prompt during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Define ONNX version as build arg with default
ARG ONNX_VERSION=1.16.3

# Install system dependencies including X11 and Qt dependencies
RUN apt-get update && apt-get install -y \
    libeigen3-dev \
    build-essential \
    cmake \
    git \
    libopencv-dev \
    wget \
    libglib2.0-0 \
    libsm6 \
    libxext6 \
    libxrender-dev \
    file \
    # X11 and Qt dependencies
    libx11-xcb1 \
    libxcb-xinerama0 \
    libxcb-randr0 \
    libxcb-xtest0 \
    libxcb-shape0 \
    libxcb-xkb1 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-render-util0 \
    libxkbcommon-x11-0 \
    libxcb-cursor0 \
    && rm -rf /var/lib/apt/lists/*

# Install ONNX Runtime - Simplified for arm64 architecture (M4 Mac)
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "aarch64" ] || [ "$ARCH" = "arm64" ]; then \
        wget https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-aarch64-${ONNX_VERSION}.tgz \
        && tar -xzf onnxruntime-linux-aarch64-${ONNX_VERSION}.tgz \
        && cp -r onnxruntime-linux-aarch64-${ONNX_VERSION}/include/* /usr/local/include/ \
        && cp -r onnxruntime-linux-aarch64-${ONNX_VERSION}/lib/* /usr/local/lib/ \
        && rm -rf onnxruntime-linux-aarch64-${ONNX_VERSION}*; \
    else \
        wget https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-x64-${ONNX_VERSION}.tgz \
        && tar -xzf onnxruntime-linux-x64-${ONNX_VERSION}.tgz \
        && cp -r onnxruntime-linux-x64-${ONNX_VERSION}/include/* /usr/local/include/ \
        && cp -r onnxruntime-linux-x64-${ONNX_VERSION}/lib/* /usr/local/lib/ \
        && rm -rf onnxruntime-linux-x64-${ONNX_VERSION}*; \
    fi && \
    echo "ONNX_VERSION=${ONNX_VERSION}" >> /etc/environment

# Update library cache
RUN ldconfig

# Create runtime directory for XDG
RUN mkdir -p /tmp/runtime-dir && chmod 700 /tmp/runtime-dir

# Create all necessary directories
RUN mkdir -p /app /app/build /app/output /app/config /app/_dataset/models /app/_dataset/videos

# Set the working directory
WORKDIR /app

# Set environment variables for X11
ENV QT_X11_NO_MITSHM=1 \
    XDG_RUNTIME_DIR=/tmp/runtime-dir

# Keep the container running by using a long-running process
CMD ["tail", "-f", "/dev/null"]