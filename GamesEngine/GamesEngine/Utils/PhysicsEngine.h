#pragma once
#include "EventHandler.h"
#include <reactphysics3d/reactphysics3d.h>

enum CollisionCategory {
	PLAYER_COLLISION = 0x0001,
	COMMON_COLLISION = 0x0002
};

class PhysicsEngine {
private:
	rp3d::Vector3 m_gravity = rp3d::Vector3(0.0, -9.81, 0.0);
	rp3d::DynamicsWorld* m_dynamicWorld;

	float m_timeStep = 0.016f; // - 1/60

	// Accumulates the time before the next physics step needs
	// to complete the physics simulation based on deltaTime
	float m_accumulator = 0.0f;

public:
	/**
	 * Singletone for the physics engine to be accesable from everywhere
	 */
	static PhysicsEngine* s_physicsEngine;

	PhysicsEngine();
	~PhysicsEngine() {
		delete this->m_dynamicWorld;
	}

	/**
	 * It is used to update the simulations for all the rigid bodies in the
	 * scene and get the values for the next set of positions 
	 * @param deltaTime							The time between each individual frame
	 */
	void UpdatePhysics(const float& deltaTime);

	/**
	 * Event handlers for all the event sets present in the physics engine
	 */
	void OnEvent(Event& e);

	/**
	 * Getters and setters
	 */
public:
	inline rp3d::DynamicsWorld* GetPhysicsWorld() const { return this->m_dynamicWorld; }
	inline const rp3d::Vector3& GetGravity() const { return this->m_gravity; }
	inline const float& GetTimeStep() const { return this->m_timeStep; }

	inline void SetPhysicsWorld(rp3d::DynamicsWorld* newWorld) { this->m_dynamicWorld = newWorld; }
	inline void SetGravity(rp3d::Vector3 newVector) {
		this->m_dynamicWorld->setGravity(newVector);
		this->m_gravity = newVector;
	}
	inline void SetTimeStep(const float& newTimeStep) { this->m_timeStep = newTimeStep; }
};