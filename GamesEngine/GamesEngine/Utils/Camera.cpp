#include "Camera.h"

Camera* Camera::s_camera;

Camera::Camera() {}

Camera::Camera(const glm::vec3& pos, const glm::vec3& rot, float fov, float aspect, float zNear, float zFar)
{
	this->m_perspective = glm::perspective(fov, aspect, zNear, zFar);
	this->m_cameraTransform = new Transform(pos, rot, glm::vec3(1.0f, 1.0f, 1.0f));
	this->m_editorTransform = new Transform(pos, rot, glm::vec3(1.0f, 1.0f, 1.0f));
	this->ResetView();
}
