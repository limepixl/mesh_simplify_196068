#include <cstdio>
#include <stdint.h>
#include "loader.hpp"

int main()
{
	std::vector<Triangle> mesh_data = LoadModelFromObj("suzanne_medium.obj", "resources/mesh/");
	return 0;
}