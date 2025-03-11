#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to print messages
log() {
    echo -e "${GREEN}[DEBUG]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if the executable exists and has correct permissions
EXECUTABLE="./build/object-tracking"
if [ ! -f "$EXECUTABLE" ]; then
    error "Executable not found at $EXECUTABLE"
    exit 1
fi

log "Checking executable permissions..."
ls -la "$EXECUTABLE"

# Make sure it's executable
chmod +x "$EXECUTABLE"
log "Ensured executable permissions"

# Check the configuration file
CONFIG_FILE="config/config.ini"
if [ ! -f "$CONFIG_FILE" ]; then
    warn "Configuration file not found at $CONFIG_FILE"
    # Create a sample config file if it doesn't exist
    mkdir -p config
    echo "[video]" > "$CONFIG_FILE"
    echo "source=0  # 0 for webcam, or path to video file" >> "$CONFIG_FILE"
    echo "[model]" >> "$CONFIG_FILE"
    echo "path=models/model.onnx  # Path to your ONNX model" >> "$CONFIG_FILE"
    log "Created sample config file at $CONFIG_FILE"
fi

log "Configuration file contents:"
cat "$CONFIG_FILE"

# Check dynamic library dependencies
log "Checking dynamic library dependencies..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    otool -L "$EXECUTABLE" | grep -i onnx
else
    # Linux
    ldd "$EXECUTABLE" | grep -i onnx
fi

# Check ONNX Runtime shared library
ONNX_LIB="$(brew --prefix)/opt/onnxruntime/lib/libonnxruntime.dylib"
if [ ! -f "$ONNX_LIB" ]; then
    error "ONNX Runtime shared library not found at $ONNX_LIB"
else
    log "ONNX Runtime shared library found at $ONNX_LIB"
fi

# Create a simple run script with environment variables set
cat > run.sh << EOF
#!/bin/bash
# Set up the environment for running object-tracking

# Add ONNX Runtime to the library path
export DYLD_LIBRARY_PATH="\$(brew --prefix)/opt/onnxruntime/lib:\$DYLD_LIBRARY_PATH"

# Suppress ONNX schema warnings
export ORT_LOGGING_LEVEL=2  # 0=Verbose, 1=Info, 2=Warning, 3=Error, 4=Fatal

# Disable dyld print libraries
unset DYLD_PRINT_LIBRARIES
unset DYLD_PRINT_APIS
unset DYLD_PRINT_BINDINGS

# Option 1: Completely redirect stderr (hides all error messages)
$EXECUTABLE "$CONFIG_FILE" -v 2>/dev/null

# Option 2: Selective filtering (uncomment to use instead)
# $EXECUTABLE "$CONFIG_FILE" -v 2>&1 | grep -v "Schema error\\|dyld\\[\\|move \\(loaded\\|delayed\\)"

# If it fails, exit with the same code
exit \$?
EOF

chmod +x run.sh

# Try running with strace or dtruss
log "Trying to run with debugging..."
if command -v dtruss &> /dev/null; then
    log "Using dtruss to debug (you may need to run as sudo):"
    echo "sudo dtruss -f $EXECUTABLE $CONFIG_FILE"
elif command -v strace &> /dev/null; then
    log "Using strace to debug:"
    echo "strace $EXECUTABLE $CONFIG_FILE"
fi

log "Done! Try running the program with the environment script:"
echo "./run.sh"