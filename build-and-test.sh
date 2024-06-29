#!/bin/bash

export CC=`which gcc-13`
export CXX=`which g++-13`

mkdir -p build && \
cd build && \
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 && \
make && \
ctest --verbose