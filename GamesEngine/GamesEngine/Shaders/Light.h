#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

enum LightType {
	Directional, Point
};

class Light {
private:
	float m_kc = 1.0f;		// constant
	float m_kl = 0.09f;		// linear
	float m_kq = 0.032f;	// quadratic

	glm::vec3 m_ambient, m_diffuse, m_specular;
	glm::vec3 m_position, m_direction;
	float m_radius = 1.0f;
	float m_intensity = 1.0f;

	LightType m_lightType;

public:
	Light( // D-Light
		LightType type,
		const glm::vec3& direction,
		const glm::vec3& diffuse,
		const glm::vec3& ambient,
		const glm::vec3& specular) 
		: m_lightType(type), m_diffuse(diffuse), m_ambient(ambient), m_specular(specular), m_direction(glm::normalize(direction)) {}
	Light( // P-Light
		LightType type,
		const glm::vec3& position,
		const glm::vec3& diffuse,
		const glm::vec3& ambient,
		const glm::vec3& specular,
		const float& radius,
		const float& intensity)
		: m_lightType(type), m_position(position), m_diffuse(diffuse), m_ambient(ambient), m_specular(specular), m_radius(radius), m_intensity(intensity) {
		this->m_kl = 2 / m_radius;
		this->m_kq = 1 / (m_radius * m_radius);
	}
	~Light() {}

	/**
	 * Set to the created program the uniform necesarry for the shader
	 * to build up the fragmentCol based on the lights present in the scene
	 * @param shader					The program that was bound to this specific shader
	 */
	void SetUniforms(Shader* shader);

	/**
	 * Getters and setters
	 */
public:
	inline const glm::vec3& GetDiffuse() const { return this->m_diffuse; }
	inline const glm::vec3& GetAmbient() const { return this->m_ambient; }
	inline const glm::vec3& GetSpecular() const { return this->m_specular; }
	inline const glm::vec3& GetPos() const { return this->m_position; }
	inline const glm::vec3& GetDir() const { return this->m_direction; }
	inline const LightType& GetType() const { return this->m_lightType; }
	inline const float& GetRadius() const { return this->m_radius; }
	inline const float& GetIntesity() const { return this->m_intensity; }

	inline void SetDiffuse(const glm::vec3& newDiff) { this->m_diffuse = newDiff; }
	inline void SetAmbient(const glm::vec3& newAmbient) { this->m_ambient = newAmbient; }
	inline void SetSpecular(const glm::vec3& newSpec) { this->m_specular = newSpec; }
	inline void SetPos(const glm::vec3& newPos) { this->m_position = newPos; }
	inline void SetDir(const glm::vec3& newDir) { this->m_direction = newDir; }
	inline void SetType(const LightType& newType) { this->m_lightType = newType; }
	inline void SetRadius(const float& newRad) { this->m_radius = newRad; }
	inline void SetIntesity(const float& newInt) { this->m_intensity = newInt; }
};