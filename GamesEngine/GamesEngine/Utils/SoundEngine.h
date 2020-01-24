#pragma once
#include "EventHandler.h"
#include <FMOD/includes/fmod.hpp>
#include <FMOD/includes/fmod_studio.hpp>
#include <string>
#include <map>

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef std::map<std::string, FMOD::Channel*> ChannelMap;

// Forward declarations
class AssetManager;

class SoundEngine {
private:
	FMOD::Studio::System* m_system;
	FMOD::System* m_lowerSystem;

	SoundMap m_soundMap;
	ChannelMap m_channelMap;

	bool m_successInit = true;

public:
	/**
	 * Singletone for the assetmanager to be accesable from everywhere
	 */
	static SoundEngine* s_soundEngine;

	SoundEngine();
	~SoundEngine();

	/**
	 * Initialise and start the background sound
	 */
	void InitBackgroundSample();

	/**
	 * Init event bound sound samples
	 */
	void InitEventSamples();

	/**
	 * Our sound engine needs to be updated each individual frame
	 */
	void Update();

	/**
	 * Check if there are any errors while generating any of the fModTools or 
	 * after firing a certain set of actions
	 */
	bool ErrorCheck(FMOD_RESULT result);

	/**
	 * Event handlers for all the event sets present in the sound engine
	 */
	void OnEvent(Event& e);

	void OnPlayerJump();

	/**
	 * Getters and setters
	 */
public:
	FMOD::Studio::System* GetMainSystem() { return this->m_system; }
	FMOD::System* GetLowerSystem() { return this->m_lowerSystem; }
	const SoundMap& GetEngineSoundMap() { return this->m_soundMap; }
	const ChannelMap& GetEngineChannelMap() { return this->m_channelMap; }

	template<typename T, typename U>
	T* GetFromMap(const std::string& fileName, U specificMap) {
		T* i = NULL;

		auto it = specificMap.find(fileName);
		if (it != specificMap.end()) i = it->second;

		return i;
	}

	void InitialiseNewSound(const std::string& fileName, FMOD::Sound* soundSample) {
		this->m_soundMap.insert({ fileName, soundSample });
		this->m_channelMap.insert({ fileName, nullptr });
	}
};
