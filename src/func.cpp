#include "func.hpp"

Triangle::Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2)
{
	this->v0 = v0;
	this->v1 = v1;
	this->v2 = v2;
	// this->normal = normal;

	this->edge0 = {v0, v1};
	this->edge1 = {v1, v2};
	this->edge2 = {v2, v0};
}

double CalculateCost(Edge &selected_edge, Edge &non_tri_edge, Edge &new_edge)
{
	double l1 = selected_edge.length() + non_tri_edge.length();
	double l2 = new_edge.length();
	return fabs(l1 - l2);
}

double VectorSum(std::vector<double> &vec)
{
	double sum = 0.0;
	for(size_t i = 0; i < vec.size(); i++)
		sum += vec.at(i);

	return sum;
}

void PopulateMultimap(std::unordered_multimap<glm::vec3, glm::vec3> &multimap, std::vector<Edge> &edges)
{
	// Clear the multimap if it has already been filled
	if(!multimap.empty())
		multimap.clear();

	size_t num_edges = edges.size();
	for(uint64_t i = 0; i < num_edges; i++)
	{
		Edge &edge = edges[i];
		multimap.insert({edge.v0, edge.v1});
		multimap.insert({edge.v1, edge.v0});
	}

	printf("Populated multimap with current edges.\n");
}

void FindCandidates(std::vector<Configuration> &candidates, multimap_type &multimap, size_t &num_vertices)
{
	printf("Looking for candidate configurations...\n");

	// Check each vertex if it has a valid configuration
	// to be considered as the center vertex
	for(multimap_type::iterator it = multimap.begin(); it != multimap.end(); )
	{
		glm::vec3 key = it->first;

		// Number of vertices adjacent to key (center) vertex
		size_t value_count = multimap.count(key);

		// Go to next key in the multimap
		if(value_count == 4)
		{
			Configuration tmp;

			while(it != multimap.end() && key == it->first)
			{
				tmp.edges.push_back({it->first, it->second});
				tmp.costs.push_back(0);
				tmp.new_edges.push_back({});
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
			while(it != multimap.end() && key == it->first);
		}
		
		num_vertices++;
	}

	if(candidates.empty())
	{
		printf("The mesh cannot be simplified any more, using a vertex unify operator.\n");
		system("pause");
		exit(0);
	}
	
	printf("Found %zu potential configurations.\n", candidates.size());
}