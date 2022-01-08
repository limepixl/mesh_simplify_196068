#pragma once
#include <vector>
#include <bvh/triangle.hpp>
#include <bvh/bvh.hpp>

bvh::Bvh<float> BuildBVH(std::vector<bvh::Triangle<float>> &triangles);