#!/bin/sh

# Build 'build' and 'bin' directories if needed
if [ ! -d "build" ]; then
    mkdir build
fi
if [ ! -d "bin" ]; then
    mkdir bin
fi

# Build the project
cmake -S. -Bbuild "$@"

# Use 'make' to compile the project and generate executables in 'bin' directory
make -C build