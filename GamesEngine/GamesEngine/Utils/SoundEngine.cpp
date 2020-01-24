#include "SoundEngine.h"
#include <iostream>
#include "../Objects/AssetManager.h"

SoundEngine* SoundEngine::s_soundEngine = new SoundEngine();

SoundEngine::SoundEngine() {
	// Create the Studio System object.
	this->ErrorCheck(FMOD::Studio::System::create(&this->m_system));

	// Create the main system object.
	this->ErrorCheck(this->m_system->getLowLevelSystem(&this->m_lowerSystem));

	if (this->m_successInit) {
		// Set the format of the speakers
		this->m_lowerSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_STEREO, 0);

		// Initialize FMOD Studio, which will also initialize FMOD Core
		this->m_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, NULL);
	}
}

SoundEngine::~SoundEngine() {
	/* Crashes some times, not fully functioning, never figured out why */
	// this->ErrorCheck(this->m_lowerSystem->release());
	// this->ErrorCheck(this->m_system->unloadAll());
	// this->ErrorCheck(this->m_system->release());
}

void SoundEngine::InitBackgroundSample() {
	AssetManager::s_assetManager->LoadSound("background.mp3", FMOD_LOOP_NORMAL);

	FMOD::Channel* tempChannel = this->GetFromMap<FMOD::Channel, ChannelMap>("background.mp3", this->m_channelMap);
	this->m_lowerSystem->playSound(
		this->GetFromMap<FMOD::Sound, SoundMap>("background.mp3", this->m_soundMap),
		NULL, 
		false, 
		&tempChannel
	);
}

void SoundEngine::InitEventSamples() {
	AssetManager::s_assetManager->LoadSound("jump.wav", FMOD_LOOP_OFF);
}

void SoundEngine::Update() {
	if (this->m_successInit) {
		this->m_system->update();
	}
}

bool SoundEngine::ErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << "FMOD ERROR: " << result << "\n";
		this->m_successInit = false;
		return false;
	}
	return true;
}

void SoundEngine::OnEvent(Event& e) {
	EventHandler eh(&e);

	if (this->m_successInit) {
		eh.Handle(EventTypes::JUMP, std::bind(&SoundEngine::OnPlayerJump, this));
	}
}

void SoundEngine::OnPlayerJump() {
	FMOD::Channel* tempChannel = this->GetFromMap<FMOD::Channel, ChannelMap>("jump.wav", this->m_channelMap);
	this->m_lowerSystem->playSound(
		this->GetFromMap<FMOD::Sound, SoundMap>("jump.wav", this->m_soundMap),
		NULL,
		false,
		&tempChannel
	);
}
