#!/bin/bash
# docker-build.sh - Wrapper script to build the project inside Docker container

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

log "Running object-tracking..."
docker exec -it object-tracking-env bash -c "/app/build/object-tracking /app/config/config.ini"