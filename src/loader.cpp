#include "loader.hpp"
#include <cstdint>

#include "../tinyobjloader/tiny_obj_loader.h"
#include <glm/glm.hpp>

Triangle::Triangle(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2, glm::vec3 &normal)
{
	this->v0 = v0;
	this->v1 = v1;
	this->v2 = v2;
	this->normal = normal;

	this->edge0 = {v0, v1};
	this->edge1 = {v1, v2};
	this->edge2 = {v2, v0};
}

std::vector<Triangle> LoadModelFromObj(const char *fileName, const char *path)
{
	printf("Loading %s model from path: %s\n", fileName, path);

	tinyobj::attrib_t attrib = {};
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;

	std::string finalPath = std::string(path) + std::string(fileName);
	bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, finalPath.c_str(), path);

	// Print out any warnings or errors that tinyobjloader returned
	if(!warning.empty())
		printf("WARNING: %s\n", warning.c_str());
	
	if(!error.empty())
		printf("ERROR: %s\n", error.c_str());

	if(!res)
	{
		printf("Failed to load obj file at path: %s\n", finalPath.c_str());
		exit(-1);
	}

	int64_t numVertices = (int64_t)(attrib.vertices.size() / 3);
	int64_t numNormals = (int64_t)(attrib.normals.size() / 3);
	int64_t numUVs = (int64_t)(attrib.texcoords.size() / 2);
	int32_t numMaterials = (int32_t)(materials.size());
	int32_t numShapes = (int32_t)(shapes.size());

	std::vector<Triangle> tris;
	for(int32_t shape = 0; shape < numShapes; shape++)
	{
		// This shape's indices
		// NOTE: assuming mesh is trangulated (which tinyobjloader does by default
		// if the mesh isn't triangulated fully), each face consists of 3 vertices
		int32_t numFaces = (int32_t)(shapes[shape].mesh.indices.size() / 3);

		for(int32_t face = 0; face < numFaces; face++)
		{
			tinyobj::index_t index0 = shapes[shape].mesh.indices[3 * face];
			tinyobj::index_t index1 = shapes[shape].mesh.indices[3 * face + 1];
			tinyobj::index_t index2 = shapes[shape].mesh.indices[3 * face + 2];

			// Get vertex data
			float vertex0[3];
			float vertex1[3];
			float vertex2[3];
			for(int32_t component = 0; component < 3; component++)
			{
				// Vertex indices are separate from uv and normal indices
				int v0 = index0.vertex_index;
				int v1 = index1.vertex_index;
				int v2 = index2.vertex_index;

				if(v0 < 0 || v1 < 0 || v2 < 0)
				{
					printf("INVALID INDICES!\n");
					exit(-1);
				}

				vertex0[component] = attrib.vertices[3 * v0 + component];
				vertex1[component] = attrib.vertices[3 * v1 + component];
				vertex2[component] = attrib.vertices[3 * v2 + component];
			}

			// Get normal data
			glm::vec3 nv0, nv1, nv2;
			float normalv0[3];
			float normalv1[3];
			float normalv2[3];
			if(numNormals > 0)
			{
				int64_t n0 = index0.normal_index;
				int64_t n1 = index1.normal_index;
				int64_t n2 = index2.normal_index;

				if(n0 < 0)
					n0 += numNormals;

				if(n1 < 0)
					n1 += numNormals;

				if(n2 < 0)
					n2 += numNormals;

				for(int32_t component = 0; component < 3; component++)
				{
					normalv0[component] = attrib.normals[3 * n0 + component];
					normalv1[component] = attrib.normals[3 * n1 + component];
					normalv2[component] = attrib.normals[3 * n2 + component];

					// Average out all vertex normals for each face
					nv0 = glm::vec3(normalv0[0], normalv0[1], normalv0[2]);
					nv1 = glm::vec3(normalv1[0], normalv1[1], normalv1[2]);
					nv2 = glm::vec3(normalv2[0], normalv2[1], normalv2[2]);
				}
			}
			else
			{
				// If there are no provided normals by the OBJ model,
				// then assume CCW winding order and calcualte the normals.
				// This makes each face have effectively one normal, so they
				// won't be interpolated between adjacent faces / shared vertices.

				glm::vec3 v0(vertex0[0], vertex0[1], vertex0[2]);
				glm::vec3 v1(vertex1[0], vertex1[1], vertex1[2]);
				glm::vec3 v2(vertex2[0], vertex2[1], vertex2[2]);
				
				glm::vec3 edge1 = v1 - v0;
				glm::vec3 edge2 = v2 - v0;
				
				glm::vec3 n = glm::cross(edge1, edge2);
				nv0 = n; nv1 = n; nv2 = n;
			}
			glm::vec3 normal = glm::normalize(nv0 + nv1 + nv2);

			// Create triangle
			glm::vec3 v0(vertex0[0], vertex0[1], vertex0[2]);
			glm::vec3 v1(vertex1[0], vertex1[1], vertex1[2]);
			glm::vec3 v2(vertex2[0], vertex2[1], vertex2[2]);
			Triangle tri(v0, v1, v2, normal);
			tris.push_back(tri);
		}
	}

	printf("Finished loading .obj model!\n");
	return tris;
}