# Dockerfile
FROM ubuntu:24.04

# Prevent timezone prompt during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
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
    && rm -rf /var/lib/apt/lists/*

# Install ONNX Runtime with architecture check
RUN ARCH=$(uname -m) && \
    ONNX_VERSION=1.16.3 && \
    if [ "$ARCH" = "x86_64" ]; then \
        wget https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-x64-${ONNX_VERSION}.tgz \
        && tar -xzf onnxruntime-linux-x64-${ONNX_VERSION}.tgz \
        && cp -r onnxruntime-linux-x64-${ONNX_VERSION}/include/* /usr/local/include/ \
        && cp -r onnxruntime-linux-x64-${ONNX_VERSION}/lib/* /usr/local/lib/ \
        && rm -rf onnxruntime-linux-x64-${ONNX_VERSION}* ; \
    elif [ "$ARCH" = "aarch64" ]; then \
        wget https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-aarch64-${ONNX_VERSION}.tgz \
        && tar -xzf onnxruntime-linux-aarch64-${ONNX_VERSION}.tgz \
        && cp -r onnxruntime-linux-aarch64-${ONNX_VERSION}/include/* /usr/local/include/ \
        && cp -r onnxruntime-linux-aarch64-${ONNX_VERSION}/lib/* /usr/local/lib/ \
        && rm -rf onnxruntime-linux-aarch64-${ONNX_VERSION}* ; \
    else \
        echo "Unsupported architecture: $ARCH" && exit 1; \
    fi

# Update library cache
RUN ldconfig

# Set working directory
WORKDIR /app

# Copy the source code
COPY . .

# Create build directory and build the project
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc) && \
    # Install to system directory
    cp object-tracking /usr/local/bin/ && \
    chmod +x /usr/local/bin/object-tracking

# Create necessary directories
RUN mkdir -p /app/output /app/config

# Set the working directory to /app
WORKDIR /app

CMD ["/usr/local/bin/object-tracking", "/app/config/config.ini"]