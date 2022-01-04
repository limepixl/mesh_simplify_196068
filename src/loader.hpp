#pragma once
#include <glm/vec3.hpp>
#include <vector>

struct Triangle
{
	glm::vec3 v0, v1, v2;
	glm::vec3 normal;
	glm::vec3 edge1, edge2;

	Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &normal);
};

std::vector<Triangle> LoadModelFromObj(const char *fileName, const char *path);
