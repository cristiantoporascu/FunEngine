#pragma once
#include <GLFW/glfw3.h>
#include "EventQueue.h"

// Forward declarations
class WindowDisplay;
class Scene;

class UIEngine {
private:
	bool currentKeyState[350] = { false };
	bool previousKeyState[350] = { false };

	WindowDisplay* m_graphicEngineEntity;

public:
	UIEngine(WindowDisplay* graphicEngine);
	~UIEngine() {}

	/**
	 * Update the previous state of the keys
	 */
	void UpdatePreviousState();

	/**
	 * Check the state of the certain keyboard inputs every update
	 * and sets the previous key based on release and pressed
	 */
	void CheckKeyboardInput();

	/**
	 * Based on the keyboard inputs run a certain action
	 * and push a certain state to the event queue
	 */
	void KeyboardAction();

	/**
	 * Getters and setters
	 */
public:
	inline const bool& GetKeyDown(const GLenum& keyCode) const { return this->currentKeyState[keyCode]; }
	inline const bool& GetKeyPressed(const GLenum& keyCode) const { return this->currentKeyState[keyCode] && !this->previousKeyState[keyCode]; }
	inline const bool& GetKeyRelease(const GLenum& keyCode) const { return !this->currentKeyState[keyCode] && this->previousKeyState[keyCode]; }
		    		 
	inline const bool& GetCurrentKeyState(const GLenum& keyCode) { return this->currentKeyState[keyCode]; }
	inline const bool& GetPreviousKeyState(const GLenum& keyCode) { return this->previousKeyState[keyCode]; }
};