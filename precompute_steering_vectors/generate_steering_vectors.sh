#!/bin/bash

# Script to build and run the steering vector precomputation tool
# and place the generated files in the main app's data directory

set -e  # Exit on any error

echo "Building steering vector precomputation tool..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build
make -j$(nproc)

echo "Running precomputation..."

# Run the tool
./precompute_steering_vectors

echo "Moving generated files to app data directory..."

# Create data directory in the main app if it doesn't exist
mkdir -p ../../data/steering_vectors

# Move the generated file
mv steering_vectors/steering_vectors.hpp ../../data/steering_vectors/

# Clean up
rm -rf steering_vectors

echo "Steering vector precomputation complete!"
echo "Files are available in: data/steering_vectors/"
echo ""
echo "Generated files:"
ls -la ../../data/steering_vectors/