#pragma once
#include "../Mathematics/Vertex.h"
#include "AssetManager.h"
#include "../Utils/PhysicsEngine.h"
#include <glm/glm.hpp>
#include <vector>

enum ObjectType {
	PLAYER, COMMON
};

class GameObject
{
private:
	std::vector<Mesh*> m_mesh;
	Transform* m_transform;
	rp3d::RigidBody* m_rigidBody;
	rp3d::BoxShape* m_collisionBox = nullptr;
	rp3d::SphereShape* m_collisionSphere = nullptr;
	rp3d::ProxyShape* m_proxyCollision = nullptr;

	float m_playerForce = 20000.0f;
	float m_playerSpeed = 1000.0f;

	// Check if the object is imported or not
	std::string m_importedFileName = "";

	// Active state of the gameObject
	bool m_active = true;

	// Game object type
	ObjectType m_objectType;

public:
	static CollisionCategory ConvertObjectTypeToCollision(const ObjectType& objType) {
		switch (objType) {
		case ObjectType::PLAYER:
			return CollisionCategory::PLAYER_COLLISION;
		case ObjectType::COMMON:
			return CollisionCategory::COMMON_COLLISION;
		}
	}

	static ObjectType ConvertIntToType(const int& id) {
		switch (id) {
		case 0:
			return ObjectType::PLAYER;
		case 1:
			return ObjectType::COMMON;
		}
	}

	GameObject();
	GameObject(
		Transform* transform,
		const std::string& meshFilePath,
		const GLenum& movementState,
		const glm::vec3& importedColour = glm::vec3(1.0f, 1.0f, 1.0f),
		const bool& physicsEnabled = true,
		const float& mass = 0.0f,
		const ObjectType& objectType = ObjectType::COMMON
	);
	GameObject(
		Transform* transform, 
		const std::vector<Mesh*>& mesh, 
		const bool& physicsEnabled = true, 
		const float& mass = 0.0f,
		const ObjectType& objectType = ObjectType::COMMON);
	~GameObject();

	/**
	 * Calls properties for the graphics part of the
	 * game engine in order to render the mesh on the scene
	 */
	virtual void DrawMesh();

	/**
	 * What does the object need to do when you first run the application
	 */
	virtual void Start();

	/**
	 * What does the object need to do every frame drawn
	 * -- ALSO --
	 * Updates the properties of the transform based on the update
	 * generated dynamicly by the physics engine each frame
	 */
	virtual void Update();

	/**
	 * Event handlers for all the event sets present in the gameobject
	 */
	void OnEvent(Event& e);

	void OnPlayerJump();
	void OnPlayerMoveForward();
	void OnPlayerMoveBackward();
	void OnPlayerMoveRight();
	void OnPlayerMoveLeft();

	/**
	 * Getters and setters
	 */
public:
	inline const std::vector<Mesh*>& GetMeshes() { return this->m_mesh; }
	inline Mesh* GetMeshById(const int& meshId) { return this->m_mesh[meshId]; }
	inline Transform* GetTransform() const { return this->m_transform; }
	inline const std::string& GetFileName() const { return this->m_importedFileName; }
	inline const bool& GetIsActive() const { return this->m_active; }
	inline const bool& GetIsImported() const { return this->m_importedFileName != ""; }
	inline rp3d::RigidBody* GetRigidBody() { return this->m_rigidBody; }
	inline rp3d::BoxShape* GetCollisionBox() { return this->m_collisionBox; }
	inline rp3d::SphereShape* GetCollisionSphere() { return this->m_collisionSphere; }
	inline const ObjectType& GetObjectType() const { return this->m_objectType; }

	inline void SetMeshes(const std::vector<Mesh*>& newMeshes) { this->m_mesh = newMeshes; }
	inline void SetTransform(Transform* newTransform) { this->m_transform = newTransform; }
	inline void SetFilePath(const std::string& newFilePath) { this->m_importedFileName = newFilePath; }
	inline void SetActiveState(const bool& newActiveState) { this->m_active = newActiveState; }
	inline void SetObjectType(const ObjectType& newType) { this->m_objectType = newType; }
	inline void SetBodyMass(const float& newMass) { this->m_rigidBody->setMass(rp3d::decimal(newMass)); }

	inline void SetPhysicsEnabled(const bool& newPhysicsEnabled) {
		// After removal of shape from the heap the mass is lost
		rp3d::decimal saveMass = this->m_rigidBody->getMass();
		
		if (this->m_proxyCollision != nullptr) {
			this->m_rigidBody->removeCollisionShape(this->m_proxyCollision);
			this->m_proxyCollision = nullptr;
		}
		if (this->m_collisionBox != nullptr) {
			delete this->m_collisionBox;
			this->m_collisionBox = nullptr;
		}
		if (this->m_collisionSphere != nullptr) {
			delete this->m_collisionSphere;
			this->m_collisionSphere = nullptr;
		}

		// Re-asign mass to the body
		this->m_rigidBody->setMass(saveMass);

		if (newPhysicsEnabled) {
			this->SetRigidBodyCollision();

			this->SetRigidBodyCollisionFilter();
		}
	}
	
	inline void SetRigidBodyCollision() {
		// Calculate BB based on all meshes
		glm::vec3 minVec(0.0f, 0.0f, 0.0f);
		glm::vec3 maxVec(0.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < this->m_mesh.size(); i++) {
			this->m_mesh[i]->CalculateBoundingBox(minVec, maxVec);
		}

		// Resize the bounding box based on the scale
		minVec *= this->m_transform->GetScale();
		maxVec *= this->m_transform->GetScale();

		// Half extents of the box in the x, y and z directions
		const rp3d::Vector3 halfExtents(
			(maxVec.x - minVec.x),
			(maxVec.y - minVec.y),
			(maxVec.z - minVec.z));

		// Create the specific shape based on object
		switch (this->m_objectType)
		{
		case ObjectType::PLAYER:
			this->m_collisionSphere = new rp3d::SphereShape((maxVec.x - minVec.x));
			break;
		case ObjectType::COMMON:
			this->m_collisionBox = new rp3d::BoxShape(halfExtents);
			break;
		default:
			break;
		}

		// Add the collision shape to the rigid body
		if (this->m_collisionBox != nullptr) {
			this->m_proxyCollision
				// Transform of the collision shape [ identity ]
				// Place the shape at the origin of the body local - space
				= this->m_rigidBody->addCollisionShape(this->m_collisionBox, rp3d::Transform::identity(), this->m_rigidBody->getMass());
		}
		else {
			this->m_proxyCollision
				= this->m_rigidBody->addCollisionShape(this->m_collisionSphere, rp3d::Transform::identity(), this->m_rigidBody->getMass());
		}
	}

	inline void SetRigidBodyCollisionFilter() {
		// Set the collision category of the object
		this->m_proxyCollision->setCollisionCategoryBits(GameObject::ConvertObjectTypeToCollision(this->m_objectType));

		// For each shape , we specify with which categories it
		// is allowed to collide
		switch (this->m_objectType)
		{
		case ObjectType::PLAYER:
			this->m_proxyCollision->setCollideWithMaskBits(GameObject::ConvertObjectTypeToCollision(ObjectType::COMMON));
			break;
		case ObjectType::COMMON:
			this->m_proxyCollision->setCollideWithMaskBits(GameObject::ConvertObjectTypeToCollision(ObjectType::PLAYER));
			break;
		default:
			break;
		}
	}

	inline void SetRigidBodyProperties(const float& mass, const bool& newPhysicsEnabled) {
		// Create the rigid body in the world
		this->m_rigidBody = PhysicsEngine::s_physicsEngine
			->GetPhysicsWorld()->createRigidBody(Transform::ConvertGraphicsTransformToPhysics(*this->m_transform));

		// Set rigid body mass
		this->m_rigidBody->setMass(mass);

		// Set the physic properties of the object
		this->SetPhysicsEnabled(newPhysicsEnabled);
	}

	inline void SetMaterialProperties() {
		this->m_rigidBody->getMaterial().setFrictionCoefficient(1.0);
		this->m_rigidBody->getMaterial().setBounciness(0.0);
	}
};
