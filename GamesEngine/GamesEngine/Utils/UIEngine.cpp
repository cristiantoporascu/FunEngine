#include "WindowDisplay.h"
#include "UIEngine.h"

EventQueue* EventQueue::s_eventQueue = nullptr;

UIEngine::UIEngine(WindowDisplay* graphicEngine) {
	// Initialise the eventQueue on the initialisation of the UIEngine
	EventQueue::s_eventQueue = new EventQueue();

	// Assign the entity of graphicEngine that the keyboard will use
	this->m_graphicEngineEntity = graphicEngine;

	// Ensure we can capture the keys being right
	glfwSetInputMode(graphicEngine->GetWindow(), GLFW_STICKY_KEYS, GL_TRUE);

	// Initilize the array so all the values for every key is false
	for (unsigned int i = 0; i < 350; i++)
	{
		this->currentKeyState[i] = false;
		this->previousKeyState[i] = false;
	}
}

void UIEngine::UpdatePreviousState() {
	for (unsigned int i = 0; i < 350; i++)
	{
		this->previousKeyState[i] = this->currentKeyState[i];
	}
}

void UIEngine::CheckKeyboardInput() {
	for (unsigned int i = 0; i < 350; i++)
	{
		int state = glfwGetKey(this->m_graphicEngineEntity->GetWindow(), i);
		if (state == GLFW_PRESS)
		{
			this->currentKeyState[i] = true;
		}
		if (state == GLFW_RELEASE)
		{
			this->currentKeyState[i] = false;
		}
	}
}

void UIEngine::KeyboardAction() {
	this->UpdatePreviousState();
	this->CheckKeyboardInput();

	if (this->GetKeyRelease(GLFW_KEY_TAB)) {
		EventQueue::s_eventQueue->AddEventToQueue(
			new Event(EventTypes::TOGGLE_WINDOW_MANAGER, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
		);
	}

	if (this->GetKeyRelease(GLFW_KEY_ESCAPE)) {
		EventQueue::s_eventQueue->AddEventToQueue(new Event(EventTypes::WINDOW_CLOSE, this->m_graphicEngineEntity->GetTimer()->GetPassedTime()));
	}

	/**
	 * -----------------------------
	 * ------ CAMERA EDITOR --------
	 * -----------------------------
	 */
	if (this->m_graphicEngineEntity->GetDisplayGUI() || this->m_graphicEngineEntity->GetGUIEngine()->GetIsBuilderActive()) {
		if (this->GetKeyDown(GLFW_KEY_A))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_ROTATE_LEFT, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);;
		}
		if (this->GetKeyDown(GLFW_KEY_D))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_ROTATE_RIGHT, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_W))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_MOVE_FORWARD, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_S))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_MOVE_BACKWARD, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_SPACE))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_ROTATE_UP, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_LEFT_CONTROL))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::C_ROTATE_DOWN, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
	}
	/**
	 * -----------------------------
	 * ------ CAMERA PLAYER --------
	 * -----------------------------
	 */
	else {
		if (this->GetKeyRelease(GLFW_KEY_SPACE))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::JUMP, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);;
		}
		if (this->GetKeyDown(GLFW_KEY_A))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::MOVE_LEFT, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);;
		}
		if (this->GetKeyDown(GLFW_KEY_D))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::MOVE_RIGHT, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_W))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::MOVE_FORWARD, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
		if (this->GetKeyDown(GLFW_KEY_S))
		{
			EventQueue::s_eventQueue->AddEventToQueue(
				new Event(EventTypes::MOVE_BACKWARD, this->m_graphicEngineEntity->GetTimer()->GetPassedTime())
			);
		}
	}
}