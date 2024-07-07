#!/bin/bash

# build config variables
export CC=`which gcc-13`
export CXX=`which g++-13`
export CMAKE_CXX_STANDARD=20
export CMAKE_BUILD_TYPE=RelWithDebInfo

# build and test steps
mkdir -p build && \
cd build && \
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && \
make && \
ctest --verbose
