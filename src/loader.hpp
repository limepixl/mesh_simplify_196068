#pragma once
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
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

	double length()
	{
		glm::vec3 t = v1 - v0;
		return glm::length(t);
	}

	Edge operator-()
	{
		return {v1, v0};
	}
};

// NOTE: All edges have v0 as the center
// vertex of the configuration
struct Configuration
{
	std::vector<Edge> edges;
	std::vector<double> costs;
};

struct Triangle
{
	glm::vec3 v0, v1, v2;
	glm::vec3 normal;
	Edge edge0, edge1, edge2;

	Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &normal);
};

std::vector<Triangle> LoadModelFromObj(const char *fileName, const char *path);
