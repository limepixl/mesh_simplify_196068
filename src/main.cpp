#include <cstdio>
#include <stdint.h>
#include "loader.hpp"
#include <unordered_map>
#include <map>
#include <queue>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

double CalculateCost(Edge &selected_edge, Edge &non_tri_edge, Edge &new_edge)
{
	double l1 = selected_edge.length() + non_tri_edge.length();
	double l2 = new_edge.length();
	return fabs(l1 - l2);
}

inline double VectorSum(std::vector<double> &vec)
{
	double sum = 0.0;
	for(int i = 0; i < vec.size(); i++)
		sum += vec.at(i);

	return sum;
}

int main()
{
	std::vector<Triangle> mesh_data = LoadModelFromObj("sphere.obj", "resources/mesh/");
	size_t num_triangles = mesh_data.size();

	std::vector<Edge> edges;
	edges.reserve(num_triangles * 3);

	for(uint64_t i = 0; i < num_triangles; i++)
	{
		Triangle &t = mesh_data[i];
		if(std::find(edges.begin(), edges.end(), t.edge0) == edges.end() && 
		   std::find(edges.begin(), edges.end(), -t.edge0) == edges.end())
			edges.push_back(t.edge0);
		if(std::find(edges.begin(), edges.end(), t.edge1) == edges.end() && 
		   std::find(edges.begin(), edges.end(), -t.edge1) == edges.end())
			edges.push_back(t.edge1);
		if(std::find(edges.begin(), edges.end(), t.edge2) == edges.end() && 
		   std::find(edges.begin(), edges.end(), -t.edge2) == edges.end())
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
	std::vector<Configuration> candidates;

	// Check each vertex if it has a valid configuration
	// to be considered as the center vertex
	size_t num_vertices = 0;
	for(multimap_type::iterator it = edge_multimap.begin(); it != edge_multimap.end(); )
	{
		glm::vec3 key = it->first;

		// Number of vertices adjacent to key (center) vertex
		size_t value_count = edge_multimap.count(key);

		// Go to next key in the multimap
		if(value_count == 4)
		{
			Configuration tmp;

			while(it != edge_multimap.end() && key == it->first)
			{
				tmp.edges.push_back({it->first, it->second});
				tmp.costs.push_back(0);
				it++;
			}

			candidates.push_back(tmp);
		}
		else
		{
			do
			{
				it++;
			}
			while(it != edge_multimap.end() && key == it->first);
		}
		
		num_vertices++;
	}

	if(candidates.empty())
	{
		printf("The mesh cannot be simplified any more, using a vertex unify operator.\n");
		return 0;
	}

	// Using lambda to compare elements.
    auto cmp = [](Configuration &left, Configuration &right) 
	{ 
		return VectorSum(left.costs) > VectorSum(right.costs);
	};
    std::priority_queue<Configuration, std::vector<Configuration>, decltype(cmp)> queue(cmp);

	// For each configuration, calculate the cost of removing each edge
	for(Configuration &c : candidates)
	{
		std::vector<glm::vec3> edge_vertices
		{
			c.edges[0].v1,
			c.edges[1].v1,
			c.edges[2].v1,
			c.edges[3].v1,
		};

		for(int i = 0; i < 4; i++)
		{
			Edge &edge = c.edges[i];

			// Find adjacent vertices to the current outer vertex
			// (the vertex that is not the center vertex)
			glm::vec3 &v_out = edge.v1;

			std::vector<glm::vec3> other_vertices = edge_vertices;
			other_vertices.erase(std::find(other_vertices.begin(), other_vertices.end(), v_out));

			std::vector<glm::vec3> tri_verts;

			// Iterate over all values for given key v_out
			std::pair<multimap_type::iterator, multimap_type::iterator> range = edge_multimap.equal_range(v_out);
			int64_t q = std::distance(range.first, range.second);
			for (multimap_type::iterator it = range.first; it != range.second; it++)
			{
				glm::vec3 &v_tri = it->second;
				
				// If it's in the configuration
				if(v_tri != edge.v0 && std::find(other_vertices.begin(), other_vertices.end(), v_tri) != other_vertices.end())
				{
					tri_verts.push_back(v_tri);
					other_vertices.erase(std::find(other_vertices.begin(), other_vertices.end(), v_tri));
				}
			}

			if(tri_verts.size() != 2)
			{
				printf("ERROR: Didn't find 2 triangle vertices!\n");
				system("pause");
			}

			Edge non_tri_edge {edge.v0, other_vertices.back()};
			Edge new_edge {edge.v1, other_vertices.back()};

			c.costs[i] = CalculateCost(edge, non_tri_edge, new_edge);
		}

		queue.push(c);
	}
	
	printf("Successfully calculated edge costs and created the priority queue.\n");
	
	system("pause");
	return 0;
}