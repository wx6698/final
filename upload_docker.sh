#!/usr/bin/env bash
# This file tags and uploads an image to Docker Hub

# Assumes that an image is built via `run_docker.sh`

# Step 1:
# Create dockerpath
dockerpath=project4

# Step 2:  
# Authenticate & tag
docker login --username=wx6698
echo "Docker ID and Image: $dockerpath"
docker tag project4 wx6698/project4:latest
# Step 3:
# Push image to a docker repository
docker push wx6698/project4:latest