#! /bin/bash

mkdir -p bin
cmake -DCMAKE_BUILD_TYPE=Debug .
cmake --build .
