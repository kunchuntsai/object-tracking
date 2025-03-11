#!/bin/bash
# docker-build.sh - Wrapper script to build the project inside Docker container

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Function to print messages
log() {
    echo -e "${GREEN}[DOCKER-BUILD]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

# Update docker-compose.yml to use absolute paths if needed
if grep -q "../../../_dataset" docker-compose.yml; then
    log "Updating docker-compose.yml to use absolute paths..."
    sed -i.bak 's|../../../_dataset|\${PWD}/_dataset|g' docker-compose.yml
    log "docker-compose.yml updated"
fi

# Check container status
CONTAINER_ID=$(docker ps -a -q -f name=object-tracking-env)
if [ -z "$CONTAINER_ID" ]; then
    log "Container doesn't exist. Starting container..."
    docker compose up -d
elif [ "$(docker inspect -f '{{.State.Running}}' object-tracking-env 2>/dev/null)" != "true" ]; then
    log "Container exists but is not running. Starting container..."
    docker start object-tracking-env
    # Wait a moment for container to initialize
    sleep 2
else
    log "Container is already running."
fi

# Verify container is actually running
if [ "$(docker inspect -f '{{.State.Running}}' object-tracking-env 2>/dev/null)" != "true" ]; then
    error "Failed to start container. Check docker logs with: docker logs object-tracking-env"
fi

# Create necessary directories
log "Ensuring directories exist..."
docker exec object-tracking-env bash -c "mkdir -p /app/_dataset/models /app/_dataset/videos /app/config"

# Parse command line arguments to pass through to build.sh
ARGS=""
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--version|-t|--type)
            ARGS="$ARGS $1 $2"
            shift 2
            ;;
        *)
            error "Unknown option: $1"
            ;;
    esac
done

# Execute build script inside the container
log "Building inside Docker container..."
docker exec object-tracking-env bash -c "cd /app && ./scripts/build.sh $ARGS"

# Check exit status of the previous command
if [ $? -ne 0 ]; then
    error "Build failed inside container"
fi

# Check if user wants to run the application after building
read -p "Run the application now? (y/n): " run_app
if [[ $run_app == "y" || $run_app == "Y" ]]; then
    log "Running object-tracking using docker-run.sh..."
    # Call docker-run.sh instead of directly running the application
    ./scripts/docker-run.sh
fi

log "Done!"