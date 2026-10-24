#!/bin/bash

# Navigate to the website directory
cd ..
cd site

# Clean previous firmware files
rm -rf /site/public/firmware

# Run the Python script in the tools directory to download firmnware manifests
python3 ../tools/manifests.py --output-dir public/firmware

# Default action if no parameter is passed
ACTION=${1:-dev}

# Check the first argument passed to the script
if [ "$ACTION" == "dev" ]; then
  echo "Starting development server..."
  # Start the development server with Vite
  npm run dev
elif [ "$ACTION" == "build" ]; then
  echo "Building the project..."
  # Run the production build
  npm run build
elif [ "$ACTION" == "clean" ]; then
  echo "Cleaning the build directory..."
  # Clean the dist directory
  rm -rf dist
elif [ "$ACTION" == "full" ]; then
  echo "Cleaning the build directory..."
  # Run full site build
  npm run full
else
  echo "Usage: ./build.sh [dev|build|clean]"
  exit 1
fi
