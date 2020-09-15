#!/usr/bin/env bash
# This file tags and uploads an image to Docker Hub

# Assumes that an image is built via `run_docker.sh`

# Step 1:
# Create dockerpath
aws ecr get-login-password --region us-east-1 | docker login --username AWS --password-stdin 164754508673.dkr.ecr.us-east-1.amazonaws.com
docker build -t test .
docker tag test:latest 164754508673.dkr.ecr.us-east-1.amazonaws.com/test:latest
docker push 164754508673.dkr.ecr.us-east-1.amazonaws.com/test:latest