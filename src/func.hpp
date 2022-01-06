#pragma once
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <map>
#include <unordered_map>
typedef std::unordered_multimap<glm::vec3, glm::vec3> multimap_type;

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <queue>

/*
	Utility structs
*/

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
	std::vector<Edge> new_edges;
};

struct Triangle
{
	glm::vec3 v0, v1, v2;
	Edge edge0, edge1, edge2;

	Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);
};

/* 
	Utility functions
*/

double CalculateCost(Edge &selected_edge, Edge &non_tri_edge, Edge &new_edge);
double VectorSum(std::vector<double> &vec);
void PopulateMultimap(multimap_type &multimap, std::vector<Edge> &edges);
void FindCandidates(std::vector<Configuration> &candidates, multimap_type &multimap, size_t &num_vertices);