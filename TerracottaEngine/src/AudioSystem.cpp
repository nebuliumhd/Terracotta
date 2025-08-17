#include "spdlog/spdlog.h"
#include "AudioSystem.hpp"

static void ERRCHECK_fn(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK) {
		SPDLOG_ERROR("FMOD Error on Line {}, Code: {}", line, static_cast<int>(result));
	}
}
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__);

namespace TerracottaEngine
{
AudioSystem::AudioSystem(SubsystemManager& manager) : Subsystem(manager)
{
	SPDLOG_INFO("Initializing the audio system...");
	// Creates BOTH the studio and core
	ERRCHECK(FMOD::Studio::System::create(&m_studioSystem));
	ERRCHECK(m_studioSystem->getCoreSystem(&m_coreSystem));
	ERRCHECK(m_coreSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE_STEREO, 0));
	// Do 3D settings later
	ERRCHECK(m_studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
}
AudioSystem::~AudioSystem()
{
	ERRCHECK(m_studioSystem->unloadAll());
	ERRCHECK(m_studioSystem->release());
}

bool AudioSystem::Init()
{
	return true;
}
void AudioSystem::OnUpdate(const float deltaTime)
{
	ERRCHECK(m_studioSystem->update());
}
void AudioSystem::Shutdown()
{

}

void AudioSystem::PlayAudio(const char* audioFilePath)
{
	FMOD::Sound* sound;
	ERRCHECK(m_coreSystem->createSound(audioFilePath, FMOD_DEFAULT, nullptr, &sound));
	FMOD::Channel* channel;
	ERRCHECK(m_coreSystem->playSound(sound, nullptr, false, &channel));
}
}