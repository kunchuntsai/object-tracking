#!/bin/bash
# Set up the environment for running object-tracking

# Add ONNX Runtime to the library path
export DYLD_LIBRARY_PATH="$(brew --prefix)/opt/onnxruntime/lib:$DYLD_LIBRARY_PATH"

# Suppress ONNX schema warnings
export ORT_LOGGING_LEVEL=2  # 0=Verbose, 1=Info, 2=Warning, 3=Error, 4=Fatal

# Disable dyld print libraries
unset DYLD_PRINT_LIBRARIES
unset DYLD_PRINT_APIS
unset DYLD_PRINT_BINDINGS

# Option 1: Completely redirect stderr (hides all error messages)
./build/object-tracking "config/config.ini" -v 2>/dev/null

# Option 2: Selective filtering (uncomment to use instead)
# ./build/object-tracking "config/config.ini" -v 2>&1 | grep -v "Schema error\|dyld\[\|move \(loaded\|delayed\)"

# If it fails, exit with the same code
exit $?
