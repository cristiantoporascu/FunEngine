#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "../Mathematics/Vertex.h"
#include "Material.h"

class Mesh
{
private:
	enum
	{
		POSITION_VBO,
		TEXTURE_COORD,
		NORMAL_VB,
		COLOUR_VB,
		TANGENT_VB,
		BITANGENT_VB,

		INDEX_VB,

		NUMBER_BUFFERS
	};

	Material* m_material;

	// Lists of vertices and indices
	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	// Id for the VAO
	GLuint m_vertexArrayObject;

	// Id for the VBO
	GLuint m_vertexBufferObject[NUMBER_BUFFERS];

	// Number of the triagnles which need to be drawn
	unsigned int m_drawCount;

	// Movement state
	GLenum m_movementState;

public:
	Mesh() {}
	Mesh(Material* material, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const GLenum& movementState);
	~Mesh();

	/**
	 * Use the specific indices or vertices for drawing the VAO
	 */
	void DrawMesh();

	/**
	 * Binds the data on the specific layout location and entry point on the shader
	 */
	void SetArrayData();

	/**
	 * Binds the specific data to the right buffer
	 * @param bufferObject					Buffer location for bufferData
	 * @param data							Buffer data casted in void*
	 * @param size							Necesary size for the buffer
	 * @param indices						Whether it should bind to the array or to the buffer
	 */
	void SetBufferData(GLuint bufferObject, const void* data, unsigned int size, bool indices = false);

	/**
	 * Resets the array buffer data on request based on the changes made to vertices data
	 */
	void ResetArrayBufferData();

	/**
	 * Calculate min and max value for the bounding box
	 * @param Ref min					Vector passed by refference to be able to change it
	 * @param Ref max				    ^_^
	 */
	void CalculateBoundingBox(glm::vec3& min, glm::vec3& max);

	/**
	 * Getters and setters
	 */
public:
	inline Material* GetMeshMaterial() const { return this->m_material; }
	inline GLuint* GetVertexBufferObject() const { return (GLuint*)(&this->m_vertexBufferObject[0]); }
	inline const std::vector<Vertex>& GetVertices() const { return this->m_vertices; }
	inline const std::vector<unsigned int>& GetIndices() const { return this->m_indices; }
	inline const GLenum& GetMovementState() const { return this->m_movementState; }

	inline void SetMeshMaterial(Material* newMaterial) { this->m_material = newMaterial; }
	inline void SetVertices(const std::vector<Vertex>& newVertices) { this->m_vertices = newVertices; }
	inline void SetIndices(const std::vector<unsigned int>& newIndices) { this->m_indices = newIndices; }
	inline void SetMovementState(const GLenum& newMovingState) { this->m_movementState = newMovingState; }
	inline void SetMovementState(const GLenum& newMovingState, rp3d::RigidBody* parentGOrb) { 
		this->m_movementState = newMovingState; 

		switch (newMovingState) {
		case GL_STATIC_DRAW:
			parentGOrb->setType(rp3d::BodyType::STATIC);
			break;
		case GL_DYNAMIC_DRAW:
			parentGOrb->setType(rp3d::BodyType::DYNAMIC);
			break;
		}
	}
};