# Dockerfile
FROM --platform=$TARGETPLATFORM ubuntu:24.04

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

# Install ONNX Runtime with platform-aware architecture detection
RUN ARCH=$(uname -m) && \
    if [ "$ARCH" = "x86_64" ]; then \
        ONNX_ARCH="x64"; \
    elif [ "$ARCH" = "aarch64" ]; then \
        ONNX_ARCH="aarch64"; \
    else \
        echo "Unsupported architecture: $ARCH" && exit 1; \
    fi && \
    wget https://github.com/microsoft/onnxruntime/releases/download/v${ONNX_VERSION}/onnxruntime-linux-${ONNX_ARCH}-${ONNX_VERSION}.tgz \
    && tar -xzf onnxruntime-linux-${ONNX_ARCH}-${ONNX_VERSION}.tgz \
    && cp -r onnxruntime-linux-${ONNX_ARCH}-${ONNX_VERSION}/include/* /usr/local/include/ \
    && cp -r onnxruntime-linux-${ONNX_ARCH}-${ONNX_VERSION}/lib/* /usr/local/lib/ \
    && rm -rf onnxruntime-linux-${ONNX_ARCH}-${ONNX_VERSION}* \
    && echo "ONNX_VERSION=${ONNX_VERSION}" >> /etc/environment

# Update library cache
RUN ldconfig

# Set working directory
WORKDIR /app

# Create runtime directory for XDG
RUN mkdir -p /tmp/runtime-dir && chmod 700 /tmp/runtime-dir

# Copy source code and scripts
COPY . .

# Make build script executable and run it
RUN chmod +x scripts/build.sh && \
    ./scripts/build.sh && \
    cp build/object-tracking /usr/local/bin/ && \
    chmod +x /usr/local/bin/object-tracking

# Create necessary directories
RUN mkdir -p /app/output /app/config

# Set the working directory to /app
WORKDIR /app

# Set default environment variables for X11
ENV QT_X11_NO_MITSHM=1 \
    XDG_RUNTIME_DIR=/tmp/runtime-dir

CMD ["/usr/local/bin/object-tracking", "/app/config/config.ini"]