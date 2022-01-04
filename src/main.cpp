#include <cstdio>
#include <stdint.h>
#include "loader.hpp"
#include <unordered_map>
#include <map>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

int main()
{
	std::vector<Triangle> mesh_data = LoadModelFromObj("suzanne_medium.obj", "resources/mesh/");
	size_t num_triangles = mesh_data.size();

	std::vector<Edge> edges;
	edges.reserve(num_triangles * 3);

	for(uint64_t i = 0; i < num_triangles; i++)
	{
		Triangle &t = mesh_data[i];
		edges.push_back(t.edge0);
		edges.push_back(t.edge1);
		edges.push_back(t.edge2);
	}

	size_t num_edges = edges.size();

	typedef std::unordered_multimap<glm::vec3, glm::vec3> multimap_type;

	// Create a map that, for any given vertex, returns all of 
	// its neighbors (vertices connected to the current vertex
	// by an edge). TODO: possibly only just use indices
	multimap_type edge_multimap;
	for(uint64_t i = 0; i < num_edges; i++)
	{
		Edge &edge = edges[i];
		edge_multimap.insert({edge.v0, edge.v1});
		edge_multimap.insert({edge.v1, edge.v0});
	}
	
	// List of candidate center vertices
	std::vector<glm::vec3> center_candicates;

	// Check each vertex if it has a valid configuration
	// to be considered as the center vertex
	size_t num_vertices = 0;
	for(multimap_type::iterator it = edge_multimap.begin(); it != edge_multimap.end(); )
	{
		glm::vec3 key = it->first;

		// Number of vertices adjacent to key (center) vertex
		size_t value_count = edge_multimap.count(key);
		if(value_count >= 4)
		{
			center_candicates.push_back(key);
		}

		// Go to next key in the multimap
		do
		{
			it++;
		}
		while(it != edge_multimap.end() && key == it->first);
		
		num_vertices++;
	}

	return 0;
}