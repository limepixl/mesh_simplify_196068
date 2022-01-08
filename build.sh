#!/bin/sh

[ ! -d "./build-linux" ] && mkdir ./build-linux
cd ./build-linux

compiler_flags="-O2 -g3 -std=c++17 -fopenmp=libomp -I ../glm -I ../tinyobjloader -I ../bvh/include -Wall -Wextra -Werror -Wno-missing-braces -Wno-unused-variable -Wno-missing-field-initializers"
files="../src/*.cpp ../tinyobjloader/*.cc"

clang++ $compiler_flags $files -o mesh_simplify
echo "Build finished"