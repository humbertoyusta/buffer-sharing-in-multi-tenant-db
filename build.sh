#!/bin/bash

rm -rf build
conan install . --output-folder build --build missing
mkdir build
cd build || exit
cmake -G Ninja ..
ninja
