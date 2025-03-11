#!/bin/bash
# docker-run.sh - Wrapper script to run the project inside Docker container

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Running object-tracking...${NC}"

# First verify the model file actually exists in the container
echo -e "${YELLOW}Checking for model file in container...${NC}"
MODEL_EXISTS=$(docker exec object-tracking-env bash -c "ls -la /app/_dataset/models/yolov7-tiny.onnx 2>/dev/null || echo 'NOT_FOUND'")
if [[ $MODEL_EXISTS == *"NOT_FOUND"* ]]; then
    echo -e "${RED}Model file not found in container!${NC}"
    echo -e "${YELLOW}Checking host system paths...${NC}"

    # Check if model exists in the host's directory structure
    if [ -f "../_dataset/models/yolov7-tiny.onnx" ]; then
        echo -e "${GREEN}Model found on host at ../_dataset/models/yolov7-tiny.onnx${NC}"
        echo -e "${YELLOW}Copying model to container...${NC}"

        # Create directory in container if needed
        docker exec object-tracking-env bash -c "mkdir -p /app/_dataset/models"

        # Copy model to container
        docker cp "../_dataset/models/yolov7-tiny.onnx" object-tracking-env:/app/_dataset/models/

        echo -e "${GREEN}Model copied to container successfully${NC}"
    else
        echo -e "${RED}Model not found on host either at ../_dataset/models/yolov7-tiny.onnx${NC}"
        echo -e "${RED}Please ensure the model file exists before running the application${NC}"
        exit 1
    fi
fi

# Run the application with proper path transformations
docker exec -it object-tracking-env bash -c '
# Create a temporary config file with Docker paths
# This pattern handles paths with or without trailing slashes
sed -e "s|path = \.\./\_dataset|path = /app/_dataset|g" \
    -e "s|video_path = \.\./\_dataset|video_path = /app/_dataset|g" \
    /app/config/config.ini > /tmp/docker-config.ini

echo "Using modified config with Docker paths:"
cat /tmp/docker-config.ini
# Add a blank line for readability
echo ""
echo ""

# Execute the application with the appropriate config path
/app/build/object-tracking "/tmp/docker-config.ini"
'
