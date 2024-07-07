#!/bin/bash

# build config variables
export CC=`which gcc-13`
export CXX=`which g++-13`
export CMAKE_CXX_STANDARD=20
export CMAKE_BUILD_TYPE=RelWithDebInfo

export BUILD_DIR=build/${CMAKE_CXX_STANDARD}-${CMAKE_BUILD_TYPE}
# build and test steps
mkdir -p ${BUILD_DIR} && \
cd ${BUILD_DIR} && \
cmake ../../ -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && \
make && \
ctest --verbose
