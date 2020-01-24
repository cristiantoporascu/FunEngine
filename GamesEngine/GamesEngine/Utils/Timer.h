#pragma once
#include <GLFW/glfw3.h>
#include <string>

class Timer
{
private:
	// Last calculated SDL_GetTicks
	float m_oldFrameTime;
	float m_newFrameTime;

	// Total frames rendered
	float m_frameCount;

	// The value of FPS
	float m_framesPerSecond;

	// Time between the frames that are rendered
	float m_frameTime;

	// Time between 2 frames
	float m_deltaTime;

	// Passed time
	float m_passedTime;

	// Profilling subSystems data
	float ProfilingStart;
	float ProfilingEnd;

	float GraphicsProfilingData;			// -- "GraphicSystem"
	float PhysicsProfilingData;				// -- "PhysicsSystem"
	float SoundProfilingData;				// -- "SoundSystem"
	float EventQueueProfilingData;			// -- "EventQueue"

public:
	Timer();
	~Timer() {}

	/**
	 * Calculate the framerate present in the scene
	 */
	void CalculateFrameRate();

	/**
	 * Start the timer for the profiling in "ms" of each subSystem
	 */
	void StartProfiling() {
		this->ProfilingStart = glfwGetTime() * 1000.0f;
	}

	/**
	 * Stop the timer for the profiling of a specific subsystem
	 * @param subSystemName						Name of the subSystem that the profiler
	 *											calculated the timeDelta on
	 */

	void StopProfiling(const std::string& subSystemName) {
		this->ProfilingEnd = glfwGetTime() * 1000.0f;

		if (subSystemName == "GraphicSystem") {
			this->GraphicsProfilingData = this->ProfilingEnd - this->ProfilingStart;
		}
		else if (subSystemName == "PhysicsSystem") {
			this->PhysicsProfilingData = this->ProfilingEnd - this->ProfilingStart;
		}
		else if (subSystemName == "SoundSystem") {
			this->SoundProfilingData = this->ProfilingEnd - this->ProfilingStart;
		}
		else if (subSystemName == "EventQueue") {
			this->EventQueueProfilingData = this->ProfilingEnd - this->ProfilingStart;
		}
	}

	/**
	 * Getters and setters
	 */
public:
	inline const float& GetFrameRate() const { return this->m_framesPerSecond; }
	inline const float& GetDeltaTime() const { return this->m_deltaTime; }
	inline const float& GetPassedTime() const { return this->m_passedTime; }

	inline const float& GetProfilingDataByName(const std::string& subSystemName) {
		
		if (subSystemName == "GraphicSystem") {
			return this->GraphicsProfilingData;
		}
		else if (subSystemName == "PhysicsSystem") {
			return this->PhysicsProfilingData;
		}
		else if (subSystemName == "SoundSystem") {
			return this->SoundProfilingData;
		}
		else if (subSystemName == "EventQueue") {
			return this->EventQueueProfilingData;
		}
	}
};