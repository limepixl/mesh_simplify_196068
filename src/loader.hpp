#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include <string>

struct Edge
{
	glm::vec3 v0;
	glm::vec3 v1;

	Edge() = default;
	bool operator==(const Edge& e)
	{
		return v0 == e.v0 && v1 == e.v1;
	}
};

struct Triangle
{
	glm::vec3 v0, v1, v2;
	glm::vec3 normal;
	Edge edge0, edge1, edge2;

	Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &normal);
};

std::vector<Triangle> LoadModelFromObj(const char *fileName, const char *path);
