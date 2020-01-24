#pragma once
#include <glm/glm.hpp>
#include "../Shaders/Shader.h"
#include "Texture.h"

class Material {
private:
	Shader* m_shader;
	std::vector<Texture*> m_textures;
	glm::vec3 m_diffuseColour = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_specularColour = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_emissionColour = glm::vec3(0.0f, 0.0f, 0.0f);
	float m_shininess = 128.0f;

public:
	Material(Shader* shader) : m_shader(shader) {}
	Material(const std::vector<Texture*>& textures, Shader* shader) : m_textures(textures), m_shader(shader) {}
	~Material() {
		delete m_shader;

		/*for (Texture* texture : m_textures) -- Commented as the Resource Manager should deal with this
			delete texture;*/
	}

	/**
	 * Bind material which will action the binding of both the texture
	 * and the shader, exporting at the same time the necesarry uniforms
	 * @param go_transform							The transform of the specific object that the
	 *												material belogs to
	 */
	void BindMaterial(const Transform& go_transform);

	/**
	 * Unbind material which will action the unbinding of both the texture and the shader
	 */
	void UnbindMaterial();

	/**
	 * Getters and setters
	 */
public:
	inline Shader* GetShader() const { return this->m_shader; }
	inline std::vector<Texture*> GetTextures() const { return this->m_textures; }
	inline const glm::vec3& GetDiffuseColour() const { return this->m_diffuseColour; }
	inline const glm::vec3& GetSpecularColour() const { return this->m_specularColour; }
	inline const glm::vec3& GetEmissionColour() const { return this->m_emissionColour; }
	inline const float& GetShininess() const { return this->m_shininess; }

	inline void SetShader(Shader* newShader) { this->m_shader = newShader; }
	inline void SetTextures(const std::vector<Texture*>& newTextures) { this->m_textures = newTextures; }
	inline void SetDiffuseColour(const glm::vec3& newColour) { this->m_diffuseColour = newColour; }
	inline void SetSpecularColour(const glm::vec3& newColour) { this->m_specularColour = newColour; }
	inline void SetEmissionColour(const glm::vec3& newColour) { this->m_emissionColour = newColour; }
	inline void SetShininess(const float& newShininess) { this->m_shininess = newShininess; }
};