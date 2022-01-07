#include "exporter.hpp"
#include "func.hpp"
#include <cstdio>

#include <glm/glm.hpp>
#include <unordered_set>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

void ExportToOBJ(const char *obj_path, std::vector<Triangle> &triangles)
{
	std::unordered_set<glm::vec3> vertex_set;
	vertex_set.reserve(triangles.size() * 3); // worst case scenario

	for(Triangle &t : triangles)
	{
		vertex_set.insert(t.v0);
		vertex_set.insert(t.v1);
		vertex_set.insert(t.v2);
	}

	std::vector<glm::vec3> vertices;
	vertices.reserve(vertex_set.size());
	vertices.insert(vertices.end(), vertex_set.begin(), vertex_set.end());

	FILE *output_file = fopen(obj_path, "w+");
	if(output_file == NULL)
	{
		printf("Couldn't write to file at path: %s\n", obj_path);
		return;
	}

	for(glm::vec3 &v : vertices)
	{
		fprintf(output_file, "v %f %f %f\n", v.x, v.y, v.z);
	}

	for(Triangle &t : triangles)
	{
		auto it0 = std::find(vertices.begin(), vertices.end(), t.v0);
		auto it1 = std::find(vertices.begin(), vertices.end(), t.v1);
		auto it2 = std::find(vertices.begin(), vertices.end(), t.v2);

		unsigned long u0 = (unsigned long)(it0 - vertices.begin());
		unsigned long u1 = (unsigned long)(it1 - vertices.begin());
		unsigned long u2 = (unsigned long)(it2 - vertices.begin());
		fprintf(output_file, "f %lu %lu %lu\n", u0 + 1, u1 + 1, u2 + 1);
	}

	fclose(output_file);
	printf("Successfully written simplified mesh into %s\n", obj_path);
}