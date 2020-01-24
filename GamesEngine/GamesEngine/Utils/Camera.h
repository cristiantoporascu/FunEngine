#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "../Mathematics/Transform.h"


class Camera
{
private:
	glm::mat4 m_perspective;
	glm::mat4 m_view;

	bool m_editorActive = true;

	Transform* m_cameraTransform;
	Transform* m_editorTransform;

public:
	/**
	 * Singletone for the camera to be accesable from everywhere
	 */
	static Camera* s_camera;

	const float TURNING_SPEED = 90.0f;
	const float MOVEMENT_SPEED = 20.0f;

	Camera();
	Camera(const glm::vec3& pos, const glm::vec3& rot, float fov, float aspect, float zNear, float zFar);
	~Camera() {
		delete m_cameraTransform;
		delete m_editorTransform;
	}

	/**
	 * Getters and setters
	 */
public:
	inline Transform* GetCurrentActiveTransform() const { return (this->m_editorActive ? this->m_editorTransform : this->m_cameraTransform);  }
	inline Transform* GetTransform() const { return this->m_cameraTransform; }
	inline Transform* GetEditorTransform() const { return this->m_editorTransform; }
	inline const glm::mat4& GetProjection() const { return this->m_perspective; }
	inline const glm::mat4& GetView() const { return this->m_view; }
	
	inline void SetCurrentActiveTransform(Transform* newTransform) { 
		if (this->m_editorActive) 
			this->m_editorTransform = newTransform;
		else 
			this->m_cameraTransform = newTransform; 
	}
	inline void SetTransform(Transform* newTransform) { this->m_cameraTransform = newTransform; }
	inline void SetEditorTransform(Transform* newTransform) { this->m_editorTransform = newTransform; }
	inline void SetProjection(const glm::mat4& newProj) { this->m_perspective = newProj; }
	inline void SetView(const glm::mat4& newView) { this->m_view = newView; }
	inline void SetEditorActiveState(const bool& newState) { this->m_editorActive = newState; }

	inline void Set3rdPersonCamera(Transform* playerPos) { 
		this->m_cameraTransform
			->SetPos(
				glm::vec3(playerPos->GetPos().x, playerPos->GetPos().y + 5.0f, playerPos->GetPos().z - 20.0f)
			); 
		this->ResetView();
	}

	inline void ResetView() {
		this->m_view = glm::lookAt(this->GetCurrentActiveTransform()->GetPos(),
			this->GetCurrentActiveTransform()->GetPos() + this->GetCurrentActiveTransform()->GetForwardDirection(),
			this->GetCurrentActiveTransform()->GetUpDirection());
	}
};