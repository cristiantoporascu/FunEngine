#include "WindowDisplay.h"
#include <glad/glad.h>

WindowDisplay::WindowDisplay(int width, int height, const std::string& title)
{
	// Set up the variables so can be accesed later
	this->m_widthScreen = width;
	this->m_heightScreen = height;

	// Initialize all the components from GLFW
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set whether the windowed mode window will be resizable by the user
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Set the desired bit depths of the various component
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_REFRESH_RATE, INT_MAX);

	// Set whether the framebuffer should be double buffered
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	// Create a window
	this->m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	// If the window is null terminate the process
	if (this->m_window == NULL)
	{
		std::cout << "ERROR: GLFW wasn't able to create the window!!" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(this->m_window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR: Failed to initialize GLAD" << std::endl;
		return;
	}

	// Init the ImGUI-GUIEngine instance
	this->m_guiEngine = new GUIEngine(this);

	// Initialise profiler for the GUIEngine instance
	// FPS Global System
	this->m_guiEngine->PushSystemProfiler(WM_SubsystemProfiling{ "FPS-Counter", 100.0f });

	// SubSystem Graphics
	this->m_guiEngine->PushSystemProfiler(WM_SubsystemProfiling{ "GraphicSystem", 0.5f });

	// SubSystem Physics
	this->m_guiEngine->PushSystemProfiler(WM_SubsystemProfiling{ "PhysicsSystem", 0.1f });
	
	// SubSystem Sound
	this->m_guiEngine->PushSystemProfiler(WM_SubsystemProfiling{ "SoundSystem", 0.1f });

	// SubSystem EventQueue
	this->m_guiEngine->PushSystemProfiler(WM_SubsystemProfiling{ "EventQueue", 0.1f });


	// Init the keyboard input
	this->m_uiEngine = new UIEngine(this);

	// Init the fps with the window
	this->m_timer = new Timer();

	// Init SkyBox
	this->windowSkyBox = new SkyBox(500);

	// Initialise the background sound
	SoundEngine::s_soundEngine->InitBackgroundSample();

	// Initialise sound for specific events
	SoundEngine::s_soundEngine->InitEventSamples();

	// Initalise the window entity for the network adapter
	NetworkEngine::s_networkEngine->SetWindowEntity(this);
}

WindowDisplay::~WindowDisplay()
{
	// Clean the GUIEngine
	delete this->m_guiEngine;

	// Clean the SkyBox
	delete this->windowSkyBox;

	// Clean the loaded assets
	delete AssetManager::s_assetManager;

	// Clean the Physics engine
	delete PhysicsEngine::s_physicsEngine;

	// Clean the sound engine
	delete SoundEngine::s_soundEngine;

	// Clean the network engine
	delete NetworkEngine::s_networkEngine;

	// Clean the UI engine
	delete this->m_uiEngine;

	// Clean the context and destroy de window
	glfwDestroyWindow(this->m_window);

	// Stops initialising everything
	glfwTerminate();
}

void WindowDisplay::UpdateWindow()
{
	// Update the frameRate
	this->m_timer->CalculateFrameRate();

	// Resize the window and the viewport
	this->ResizeWindow();

	// Update the keyboard input based on the event pool
	this->m_uiEngine->KeyboardAction();

	// Update event pool with events that come from the network adapter
	NetworkEngine::s_networkEngine->UpdateEventPool(this->m_timer->GetDeltaTime());

	// Run update for all game objects.
	if (!this->m_guiEngine->GetIsBuilderActive()) {
		// Update the physics and check for collisions only when the level
		// builder is not activated
		this->m_timer->StartProfiling();
		PhysicsEngine::s_physicsEngine
			->UpdatePhysics(this->m_timer->GetDeltaTime());
		this->m_timer->StopProfiling("PhysicsSystem");

		// If the level hasn't been selected yet don't draw any objects
		this->m_timer->StartProfiling();
		if (this->m_sceneManager != nullptr) {
			this->m_sceneManager->DrawScene(false);
		}
		this->m_timer->StopProfiling("GraphicSystem");

		// Draw the data received by the remote host or client
		NetworkEngine::s_networkEngine->DrawRemoteData();
	}
	else {
		this->m_timer->StartProfiling();
		this->m_guiEngine->GUI_LevelBuilder_ScenePreview();
		this->m_timer->StopProfiling("GraphicSystem");
	}

	// Update the sound system every frame
	this->m_timer->StartProfiling();
	SoundEngine::s_soundEngine->Update();
	this->m_timer->StopProfiling("SoundSystem");

	// Render the SkyBox
	this->windowSkyBox->DrawSkyBox();

	// Render all elements from GUIEngine
	this->m_guiEngine->DrawGUI(this->m_displayGUI);

	// Call the event handlers each frame for each subsystem
	this->m_timer->StartProfiling();
	for (unsigned int i = 0; i < EventQueue::s_eventQueue->GetEventQueueSize(); i++) {
		PhysicsEngine::s_physicsEngine->OnEvent(*EventQueue::s_eventQueue->GetEventById(i));
		SoundEngine::s_soundEngine->OnEvent(*EventQueue::s_eventQueue->GetEventById(i));
		NetworkEngine::s_networkEngine->OnEvent(*EventQueue::s_eventQueue->GetEventById(i));
		this->OnEvent(*EventQueue::s_eventQueue->GetEventById(i));

		// If the level hasn't been selected yet don't fire any event
		if (this->m_sceneManager != nullptr) {
			this->m_sceneManager->HandleEvents(*EventQueue::s_eventQueue->GetEventById(i));
		}
	}
	this->m_timer->StopProfiling("EventQueue");

	// After all updates, clear the EventQueue
	EventQueue::s_eventQueue->ClearHandledEvents();

	// Update the seed of the data packets after each frame
	NetworkEngine::s_networkEngine->UpdatePacketData();

	// Update camera based on players action
	if (this->m_sceneManager != nullptr) {
		Camera::s_camera->Set3rdPersonCamera(this->m_sceneManager->GetScenePlayer()->GetTransform());
	}

	// Update and push data in the profiler for display
	if (this->m_guiEngine->GetIsProfilerActive()) {
		// FPS Global System
		this->m_guiEngine
			->PushProfilerDataSetByName("FPS-Counter", this->m_timer->GetFrameRate());

		// SubSystem Graphics
		this->m_guiEngine->PushProfilerDataSetByName("GraphicSystem", this->m_timer->GetProfilingDataByName("GraphicSystem"));

		// SubSystem Physics
		this->m_guiEngine->PushProfilerDataSetByName("PhysicsSystem", this->m_timer->GetProfilingDataByName("PhysicsSystem"));

		// SubSystem Sound
		this->m_guiEngine->PushProfilerDataSetByName("SoundSystem", this->m_timer->GetProfilingDataByName("SoundSystem"));

		// SubSystem EventQueue
		this->m_guiEngine->PushProfilerDataSetByName("EventQueue", this->m_timer->GetProfilingDataByName("EventQueue"));

	}

	// Tells to the operating system to swap the windows on those 2 buffers and so there are no moments when nothing is drawn on the screen
	glfwSwapBuffers(this->m_window);
}

void WindowDisplay::DrawWindow()
{
	// Check if the ESC key was pressed or the window was closed
	while (this->m_windowActiveState && glfwWindowShouldClose(this->m_window) == 0)
	{
		// Check every frame if the window is focused
		// - this->FocusedWindow(); 
		// -- disabled as we can't see effects on the window of the client that is not focused

		if (this->m_windowFocusedState) {
			// Clear the window every frame
			this->ClearWindow();

			// Render what is on the window and update the instances of all the subsystems
			this->UpdateWindow();
		}

		// Check every frame if there have been any events
		glfwPollEvents();
	}

	// Clears up all the data after the window was closed
	this->CloseWindow();
}

void WindowDisplay::ResizeWindow()
{
	glfwGetFramebufferSize(this->m_window, &this->m_widthScreen, &this->m_heightScreen);
	glViewport(0, 0, this->m_widthScreen, this->m_heightScreen);

	// If the window size is seen as being 0, don't change the aspect ratio unless initialised
	if(this->m_heightScreen > 0 || this->m_widthScreen > 0)
		Camera::s_camera->SetProjection(glm::perspective(70.0f, (float)this->m_widthScreen / (float)this->m_heightScreen, 0.01f, 1000.0f));
}

void WindowDisplay::FocusedWindow() {
	if (this->m_windowFocusedState != glfwGetWindowAttrib(this->m_window, GLFW_FOCUSED))
		this->m_windowFocusedState = glfwGetWindowAttrib(this->m_window, GLFW_FOCUSED);
}

void WindowDisplay::ClearWindow() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
}

void WindowDisplay::CloseWindow() {
	delete this;
}

void WindowDisplay::OnEvent(Event& e) {
	EventHandler eh(&e);
	eh.Handle(EventTypes::C_MOVE_FORWARD, std::bind(&WindowDisplay::OnC_MoveForward, this));
	eh.Handle(EventTypes::C_MOVE_BACKWARD, std::bind(&WindowDisplay::OnC_MoveBackward, this));
	eh.Handle(EventTypes::C_ROTATE_UP, std::bind(&WindowDisplay::OnC_RotateUp, this));
	eh.Handle(EventTypes::C_ROTATE_DOWN, std::bind(&WindowDisplay::OnC_RotateDown, this));
	eh.Handle(EventTypes::C_ROTATE_LEFT, std::bind(&WindowDisplay::OnC_RotateLeft, this));
	eh.Handle(EventTypes::C_ROTATE_RIGHT, std::bind(&WindowDisplay::OnC_RotateRight, this));
	eh.Handle(EventTypes::WINDOW_CLOSE, std::bind(&WindowDisplay::OnCloseWindow, this));
	eh.Handle(EventTypes::TOGGLE_WINDOW_MANAGER, std::bind(&WindowDisplay::OnWindowManagerToggle, this));
	eh.Handle(EventTypes::PLAYER_FINISH, std::bind(&WindowDisplay::OnLevelFinish, this));
}

void WindowDisplay::OnLevelFinish() {
	this->SetSceneManager(this->m_guiEngine->GetLevelDataById(this->m_sceneManager->GetCurrentLevel()));
}

void WindowDisplay::OnC_MoveForward() {
	Camera::s_camera->GetEditorTransform()
		->SetPos(Camera::s_camera->GetEditorTransform()->GetPos()
			+ Camera::s_camera->GetEditorTransform()->GetForwardDirection() * (Camera::s_camera->MOVEMENT_SPEED * this->m_timer->GetDeltaTime()));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnC_MoveBackward() {
	Camera::s_camera->GetEditorTransform()->SetPos(Camera::s_camera->GetEditorTransform()->GetPos()
		- Camera::s_camera->GetEditorTransform()->GetForwardDirection() * (Camera::s_camera->MOVEMENT_SPEED * this->m_timer->GetDeltaTime()));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnC_RotateUp() {
	Camera::s_camera->GetEditorTransform()
		->SetRot(Camera::s_camera->GetEditorTransform()->GetRot()
			+ glm::vec3((Camera::s_camera->TURNING_SPEED * this->m_timer->GetDeltaTime()), 0.0f, 0.0f));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnC_RotateDown() {
	Camera::s_camera->GetEditorTransform()
		->SetRot(Camera::s_camera->GetEditorTransform()->GetRot()
			- glm::vec3((Camera::s_camera->TURNING_SPEED * this->m_timer->GetDeltaTime()), 0.0f, 0.0f));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnC_RotateLeft() {
	Camera::s_camera->GetEditorTransform()
		->SetRot(Camera::s_camera->GetEditorTransform()->GetRot()
			+ glm::vec3(0.0f, (Camera::s_camera->TURNING_SPEED * this->m_timer->GetDeltaTime()), 0.0f));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnC_RotateRight() {
	Camera::s_camera->GetEditorTransform()
		->SetRot(Camera::s_camera->GetEditorTransform()->GetRot()
			- glm::vec3(0.0f, (Camera::s_camera->TURNING_SPEED * this->m_timer->GetDeltaTime()), 0.0f));
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnWindowManagerToggle() {
	this->m_displayGUI = !this->m_displayGUI;
	Camera::s_camera->SetEditorActiveState(this->m_displayGUI || this->m_guiEngine->GetIsBuilderActive());
	Camera::s_camera->ResetView();
}

void WindowDisplay::OnCloseWindow() {
	if (NetworkEngine::s_networkEngine->GetNetworkType() == NetworkType::CLIENTS) {
		if (NetworkEngine::s_networkEngine->GetClientInstance()->peer != NULL)
		{
			enet_peer_disconnect_now(NetworkEngine::s_networkEngine->GetClientInstance()->peer, 0);
		}
	}
	this->SetActiveStateClosed();
}