#include "Vertex.h"

Vertex::Vertex() {
	this->pos = glm::vec3(0,0,0);
	this->textureCoord = glm::vec2(0, 0);
	this->colour = glm::vec3(0, 0, 0);
	this->normals = glm::vec3(0, 0, 0);
	this->tangent = glm::vec3(0, 0, 0);
	this->biTangent = glm::vec3(0, 0, 0);
}

Vertex::Vertex(const glm::vec3& position, const glm::vec2& textureCoord, const glm::vec3& normalCoord, const glm::vec3& colourVec, const glm::vec3& tangent, const glm::vec3& biTangent)
{
	this->pos = position;
	this->textureCoord = textureCoord;
	this->colour = colourVec;
	this->normals = normalCoord;
	this->tangent = tangent;
	this->biTangent = biTangent;
}

Vertex::~Vertex() {}