#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <reactphysics3d/reactphysics3d.h>

class Transform
{
private:
	glm::vec3 m_pos;
	glm::vec3 m_rot;
	glm::vec3 m_scale;

public:
	Transform();
	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	~Transform();

	/**
	 * Get model view matrix of the rotation
	 * @param checkRotation			rotation axis ( false => own axis, true => origin )
	 * @return glm::mat4			model view matrix rotation based
	 */
	glm::mat4 ModelMatrix(const bool& checkRotation) const;

	/**
	 * Permits the conversion between different game engine systems
	 * @param reactTransform			transform entity of physics system
	 * @return transform				transform entity of graphics system
	 */
	static Transform ConvertPhysicsTransformToGraphics(const rp3d::Transform& reactTransform) {
		rp3d::Vector3 reactPos = reactTransform.getPosition();
		rp3d::Quaternion reactRot = reactTransform.getOrientation();

		glm::vec3 eulerRot;

		double offsetValue = reactRot.x * reactRot.y + reactRot.z * reactRot.w;
		if (offsetValue > 0.499f) { // Singularity at north pole
			eulerRot = glm::vec3(
				0,
				2 * atan2(reactRot.x, reactRot.w),
				glm::half_pi<float>()
			);
		}
		else if (offsetValue < -0.499f) { // Singularity at south pole
			eulerRot = -glm::vec3(
				0,
				2 * atan2(reactRot.x, reactRot.w),
				glm::half_pi<float>()
			);
		}
		else {
			eulerRot = glm::vec3(
				atan2(2 * reactRot.x * reactRot.w - 2 * reactRot.y * reactRot.z, 1 - 2 * reactRot.x * reactRot.x - 2 * reactRot.z * reactRot.z),
				atan2(2 * reactRot.y * reactRot.w - 2 * reactRot.x * reactRot.z, 1 - 2 * reactRot.y * reactRot.y - 2 * reactRot.z * reactRot.z),
				asin(2 * offsetValue)
			);
		}

		return Transform(
			glm::vec3(reactPos.x, reactPos.y, reactPos.z),
			eulerRot,
			glm::vec3(1.0f, 1.0f, 1.0f)
		);
	}

	/**
	 * Permits the conversion between different game engine systems
	 * @param transform					transform entity of graphics system
	 * @return reactTransform			transform entity of physics system
	 */
	static rp3d::Transform ConvertGraphicsTransformToPhysics(const Transform& transform) {
		glm::vec3 pos = transform.GetPos();
		glm::vec3 rot = transform.GetRot();
		
		return rp3d::Transform(
			rp3d::Vector3(pos.x, pos.y, pos.z),
			rp3d::Quaternion(rp3d::Quaternion::fromEulerAngles(glm::degrees(rot.x), glm::degrees(rot.y), glm::degrees(rot.z)))
		);
	}

	/**
	 * Getters and setters
	 */
public:
	inline const glm::vec3& GetPos() const { return this->m_pos; }
	inline const glm::vec3& GetRot() const { return this->m_rot; }
	inline const glm::vec3& GetScale() const { return this->m_scale; }

	inline void SetPos(const glm::vec3& newPos) { this->m_pos = newPos; }
	inline void SetRot(const glm::vec3& newRot) { this->m_rot = newRot; }
	inline void SetScale(const glm::vec3& newScale) { this->m_scale = newScale; }

	inline glm::vec3 GetForwardDirection() const {
		return glm::normalize(glm::vec3(
			sin(glm::radians(this->m_rot.y)),
			tan(glm::radians(this->m_rot.x)),
			cos(glm::radians(this->m_rot.y))
		));
	}

	inline glm::vec3 GetRightDirection() const { 
		return glm::cross(GetForwardDirection(), glm::vec3(0, 1, 0)); 
	}
	inline glm::vec3 GetUpDirection() const {
		return glm::cross(GetForwardDirection(), -GetRightDirection());
	}
};
