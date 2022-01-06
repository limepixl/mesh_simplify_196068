#include <cstdio>
#include <stdint.h>
#include "loader.hpp"
#include <queue>
#include <algorithm>

#include "exporter.hpp"

// Using lambda to compare elements.
auto cmp = [](Configuration &left, Configuration &right) 
{ 
	return VectorSum(left.costs) > VectorSum(right.costs);
};

std::priority_queue<Configuration, std::vector<Configuration>, decltype(cmp)> 
CalculateConfigurationCost(std::vector<Configuration> &candidates, multimap_type &edge_multimap)
{
	std::priority_queue<Configuration, std::vector<Configuration>, decltype(cmp)> queue(cmp);

	// For each configuration, calculate the cost of removing each edge
	for(size_t j = 0; j < candidates.size(); j++)
	{
		Configuration &c = candidates[j];

		std::vector<glm::vec3> edge_vertices
		{
			c.edges[0].v1,
			c.edges[1].v1,
			c.edges[2].v1,
			c.edges[3].v1,
		};

		bool valid = true;
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
			// int64_t q = std::distance(range.first, range.second);
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
				// printf("Didn't find 2 triangle vertices! Configuration isn't valid.\n");
				valid = false;
				break;
			}

			Edge non_tri_edge {edge.v0, other_vertices.back()};
			Edge new_edge {edge.v1, other_vertices.back()};
			c.new_edges[i] = new_edge;

			c.costs[i] = CalculateCost(edge, non_tri_edge, new_edge);
		}

		if(valid)
			queue.push(c);
		else
			candidates.erase(candidates.begin() + j--);
	}
	
	if(candidates.empty())
		printf("None of them are valid. Queue is empty.\n");
	else
		printf("Successfully calculated edge costs and created the priority queue.\n");

	return queue;
}

template<typename QueueType>
void ProcessQueue(QueueType &queue, std::vector<Edge> &edges, std::vector<Triangle> &tris)
{
	// While the queue is full, process all candidate configurations!
	uint32_t count = 0;
	while(!queue.empty())
	{
		const Configuration c = queue.top();

		// Apply vertex unify operator to the edge with the 
		// lowest cost, within the selected configuration
		int edge = -1;
		double cost = (double)INFINITY;
		for(int i = 0; i < 4; i++)
		{
			if(c.costs[i] < cost)
			{
				cost = c.costs[i];
				edge = i;
			}
		}

		Edge new_edge = c.new_edges[edge];

		// Remove all trianges with the center vertex 
		// being one of their vertices
		int tri_count = 0;
		glm::vec3 center_vert = c.edges[0].v0;
		for(size_t i = 0; i < tris.size() && tri_count < 4; i++)
		{
			Triangle &t = tris[i];
			if(t.v0 == center_vert ||
			   t.v1 == center_vert ||
			   t.v2 == center_vert)
			{
				tris.erase(tris.begin() + i--);
				tri_count++;
			}
		}

		if(tri_count != 4)
		{
			printf("ERROR FATMAGJULLLLLLL\n");
		}

		std::vector<glm::vec3> new_tri_points
		{
			c.edges[0].v1,
			c.edges[1].v1,
			c.edges[2].v1,
			c.edges[3].v1,
		};

		{
			auto it1 = std::find(new_tri_points.begin(), new_tri_points.end(), new_edge.v0);
			if(it1 != new_tri_points.end())
				new_tri_points.erase(it1);
			else
				printf("ERROR1\n");

			auto it2 = std::find(new_tri_points.begin(), new_tri_points.end(), new_edge.v1);
			if(it2 != new_tri_points.end())
				new_tri_points.erase(it2);
			else
				printf("ERROR2\n");
		}

		tris.push_back(Triangle(new_tri_points[0], new_tri_points[1], new_edge.v0));
		tris.push_back(Triangle(new_tri_points[0], new_tri_points[1], new_edge.v1));

		// Remove all edges and add the new edge
		for(Edge e : c.edges)
		{
			auto it1 = std::find(edges.begin(), edges.end(), e);
			if(it1 != edges.end())
				edges.erase(std::find(edges.begin(), edges.end(), e));
				
			auto it2 = std::find(edges.begin(), edges.end(), -e);
			if(it2 != edges.end())
				edges.erase(std::find(edges.begin(), edges.end(), -e));

		}

		edges.push_back(new_edge);
		queue.pop();
		
		printf("Processed configuration #%d\n", ++count);
		printf("Number of edges currently: %zu\n", edges.size());
	}
}

int main()
{
	std::vector<Triangle> mesh_data = LoadModelFromObj("suzanne_medium.obj", "resources/mesh/");
	size_t num_triangles = mesh_data.size();
	printf("Loaded:\n- %zu triangles\n", num_triangles);

	std::vector<Edge> edges;
	edges.reserve(num_triangles * 3);

	for(Triangle &t : mesh_data)
	{
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
	printf("- %zu edges\n", num_edges);

	while(true)
	{
		// Create a map that, for any given vertex, returns all of 
		// its neighbors (vertices connected to the current vertex
		// by an edge).
		multimap_type edge_multimap;
		PopulateMultimap(edge_multimap, edges);
		
		// List of candidate center vertices
		std::vector<Configuration> candidates;

		size_t num_vertices = 0;
		FindCandidates(candidates, edge_multimap, num_vertices);

		if(candidates.empty())
		{
			break;
		}

		auto queue = CalculateConfigurationCost(candidates, edge_multimap);

		if(!queue.empty())
			ProcessQueue(queue, edges, mesh_data);
		else
		{
			printf("The mesh cannot be simplified any more, using a vertex unify operator.\n");
			break;
		}
	}

	// Generate the simplified mesh as its own obj
	ExportToOBJ("simplified_mesh.obj", mesh_data);

	system("pause");
	return 0;
}