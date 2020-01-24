#include "GameObject.h"

GameObject::GameObject() {
	this->m_transform = new Transform();
	this->m_mesh.push_back(new Mesh());
}

GameObject::GameObject(
	Transform* transform, 
	const std::string& meshFilePath, 
	const GLenum& movementState, 
	const glm::vec3& importedColour,
	const bool& physicsEnabled,
	const float& mass,
	const ObjectType& objectType
) {
	this->m_transform = transform;
	this->m_importedFileName = meshFilePath;
	this->m_mesh = AssetManager::s_assetManager->LoadMesh(meshFilePath, true, false, movementState, importedColour);

	this->m_objectType = objectType;

	this->SetRigidBodyProperties(mass, physicsEnabled);

	// Change the type of the rigid body
	switch (movementState) {
	case GL_STATIC_DRAW:
		this->m_rigidBody->setType(rp3d::BodyType::STATIC);
		break;
	case GL_DYNAMIC_DRAW:
		this->m_rigidBody->setType(rp3d::BodyType::DYNAMIC);
		break;
	}
	this->SetMaterialProperties();
}

GameObject::GameObject(
	Transform* transform, 
	const std::vector<Mesh*>& mesh, 
	const bool& physicsEnabled, 
	const float& mass,
	const ObjectType& objectType
) {
	this->m_transform = transform;
	this->m_mesh = mesh;

	this->m_objectType = objectType;

	this->SetRigidBodyProperties(mass, physicsEnabled);

	// Change the type of the rigid body
	switch ((mesh.size() > 0) ? mesh[0]->GetMovementState() : GL_STATIC_DRAW) {
	case GL_STATIC_DRAW:
		this->m_rigidBody->setType(rp3d::BodyType::STATIC);
		break;
	case GL_DYNAMIC_DRAW:
		this->m_rigidBody->setType(rp3d::BodyType::DYNAMIC);
		break;
	}
	this->SetMaterialProperties();
}

void GameObject::DrawMesh()
{
	if (this->m_active) {
		for (Mesh* m : this->m_mesh) {
			m->GetMeshMaterial()->BindMaterial(*this->m_transform);
			m->DrawMesh();
			m->GetMeshMaterial()->UnbindMaterial();
		}
	}
}

void GameObject::Start() {
	return;
}

void GameObject::Update() {
	// Updates the properties of the transform based on the
	// update from the physics
	Transform tempValues = Transform::ConvertPhysicsTransformToGraphics(this->m_rigidBody->getTransform());

	// Update each individual value so that we don't make
	// a new instance every time for a new transform
	this->m_transform->SetPos(tempValues.GetPos());

	// Dont need to rotate as the character is currently moving 
	// only on x-z axis with the jump ability too
	// --- this->m_transform->SetRot(tempValues.GetRot());
}

GameObject::~GameObject() {
	for (Mesh* m : this->m_mesh) {
		delete m;
	}
	delete this->m_transform;

	if (this->m_proxyCollision != nullptr) {
		this->m_rigidBody->removeCollisionShape(this->m_proxyCollision);
	}
	if (this->m_collisionBox != nullptr) {
		delete this->m_collisionBox;
	}

	PhysicsEngine::s_physicsEngine
		->GetPhysicsWorld()
		->destroyRigidBody(this->m_rigidBody);
}

void GameObject::OnEvent(Event& e) {
	EventHandler eh(&e);

	if (this->m_objectType == ObjectType::PLAYER) {
		eh.Handle(EventTypes::JUMP, std::bind(&GameObject::OnPlayerJump, this));
		eh.Handle(EventTypes::MOVE_FORWARD, std::bind(&GameObject::OnPlayerMoveForward, this));
		eh.Handle(EventTypes::MOVE_BACKWARD, std::bind(&GameObject::OnPlayerMoveBackward, this));
		eh.Handle(EventTypes::MOVE_LEFT, std::bind(&GameObject::OnPlayerMoveLeft, this));
		eh.Handle(EventTypes::MOVE_RIGHT, std::bind(&GameObject::OnPlayerMoveRight, this));
	}
}

void GameObject::OnPlayerJump() {
	this->m_rigidBody->applyForceToCenterOfMass(rp3d::Vector3(0.0, this->m_playerForce, 0.0));
}

void GameObject::OnPlayerMoveForward() {
	this->m_rigidBody->applyForceToCenterOfMass(rp3d::Vector3(0.0, 0.0, this->m_playerSpeed));
}

void GameObject::OnPlayerMoveBackward() {
	this->m_rigidBody->applyForceToCenterOfMass(rp3d::Vector3(0.0, 0.0, -this->m_playerSpeed));
}

void GameObject::OnPlayerMoveRight() {
	this->m_rigidBody->applyForceToCenterOfMass(rp3d::Vector3(-this->m_playerSpeed, 0.0, 0.0));
}

void GameObject::OnPlayerMoveLeft() {
	this->m_rigidBody->applyForceToCenterOfMass(rp3d::Vector3(this->m_playerSpeed, 0.0, 0.0));
}
