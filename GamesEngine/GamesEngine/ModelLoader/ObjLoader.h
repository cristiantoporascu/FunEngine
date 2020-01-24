#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class ObjLoader
{
public:
	static bool loadOBJ(const char* path, std::vector<glm::vec3>& inputVertices,
		std::vector<glm::vec2>& inputUvs, std::vector<glm::vec3>& inputNormalss/*, std::vector<unsigned int>& inputIndicesVertices*/);
};
