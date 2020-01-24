#pragma once
#include <glm/glm.hpp>

class Vertex
{
public:
	glm::vec3 pos;
	glm::vec2 textureCoord;
	glm::vec3 colour;
	glm::vec3 normals;
	glm::vec3 tangent;
	glm::vec3 biTangent;

	Vertex();
	Vertex(
		const glm::vec3& position, 
		const glm::vec2& textureCoord, 
		const glm::vec3& normalCoord, 
		const glm::vec3& colourVec, 
		const glm::vec3& tangent = glm::vec3(0.0f, 0.0f, 0.0f), 
		const glm::vec3& biTangent = glm::vec3(0.0f, 0.0f, 0.0f));
	~Vertex();
};

