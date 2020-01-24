#include "PhysicsEngine.h"

PhysicsEngine* PhysicsEngine::s_physicsEngine = new PhysicsEngine();

PhysicsEngine::PhysicsEngine() {
	// Initialise the value of the dynamic world
	this->m_dynamicWorld = new rp3d::DynamicsWorld(this->m_gravity);

	// Enable the sleeping technique to activate the resting of bodies that
	// are not simulated anymore for saving computing time
	this->m_dynamicWorld->enableSleeping(true);
}

void PhysicsEngine::UpdatePhysics(const float& deltaTime) {
	// Add time difference in the accumulator
	this->m_accumulator += deltaTime;

	// While there is enough accumulated time to take
	// one or several physics steps
	while (this->m_accumulator >= this->m_timeStep) {

		// Update the Dynamics world with a constant time step
		this->m_dynamicWorld->update(this->m_timeStep);

		// Decrease the accumulated time
		this->m_accumulator -= this->m_timeStep;
	}
}

void PhysicsEngine::OnEvent(Event& e) {
	EventHandler eh(&e);
}
