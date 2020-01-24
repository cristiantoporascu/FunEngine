#pragma once
#include "../SceneLoader/Scene.h"
#include "GUIEngine.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "Camera.h"
#include "Timer.h"
#include "UIEngine.h"
#include "PhysicsEngine.h"
#include "NetworkEngine.h"
#include "../Objects/SkyBox.h"

class WindowDisplay
{
private:
	GLFWwindow* m_window;
	Scene* m_sceneManager;
	Timer* m_timer;

	SkyBox* windowSkyBox;

	GUIEngine* m_guiEngine;
	UIEngine* m_uiEngine;

	int m_widthScreen;
	int m_heightScreen;

	bool m_displayGUI = true;
	bool m_windowFocusedState = true;
	bool m_windowActiveState = true;

public:
	WindowDisplay(int width, int height, const std::string& title);
	~WindowDisplay();

	/**
	 * Is used to declare the cover of all the systems for the event poll and
	 * the drawing of all the gameobjects declared inside the scene
	 */
	void UpdateWindow();

	/**
	 * Main function which is called at the start of the game
	 * executes all the other classes
	 */
	void DrawWindow();

	/**
	 * In case if the window has been resized it recreates the projection matrix
	 * for the camera and resets the sizes of the window
	 */
	void ResizeWindow();

	/**
	 * Checks every frame whether the window is focused or not
	 * and stops drawing anything on the window if it isn't
	 */
	void FocusedWindow();

	/**
	 * Clears the colour of the window
	 */
	void ClearWindow();

	/**
	 * Closes and clears up the memory used by the application
	 */
	void CloseWindow();

	/**
	 * Event handlers for all the event sets present in the graphics engine
	 */
	void OnEvent(Event& e);

	void OnLevelFinish();

	void OnC_MoveForward();
	void OnC_MoveBackward();

	void OnC_RotateLeft();
	void OnC_RotateRight();
	void OnC_RotateUp();
	void OnC_RotateDown();

	void OnWindowManagerToggle();

	void OnCloseWindow();

	/**
	 * Getters and setters
	 */
public:
	inline const int& GetScreenWidth() const { return this->m_widthScreen; }
	inline const int& GetScreenHeight() const { return this->m_heightScreen; }
	inline GLFWwindow* GetWindow() const { return this->m_window; }
	inline Scene* GetSceneManager() const { return this->m_sceneManager; }
	inline Timer* GetTimer() const { return this->m_timer; }
	inline GUIEngine* GetGUIEngine() const { return this->m_guiEngine;  }
	inline const bool& GetDisplayGUI() const { return this->m_displayGUI; }

	inline void SetScreenWidth(const int& newWidth) { this->m_widthScreen = newWidth; }
	inline void SetScreenHeight(const int& newHeight) { this->m_heightScreen = newHeight; }
	inline void SetDisplayGUI(const bool& newState) { this->m_displayGUI = newState; }
	inline void SetActiveStateClosed() { this->m_windowActiveState = false; }
	inline void SetSceneManager(Scene* newScene) { this->m_sceneManager = newScene; }
};