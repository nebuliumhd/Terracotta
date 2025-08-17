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
	m_channelGroups.fill(nullptr);
	Init(); // Remember to call this!
}
AudioSystem::~AudioSystem()
{
	Shutdown();
}

bool AudioSystem::Init()
{
	SPDLOG_INFO("Initializing the audio system...");
	// Creates BOTH the studio and core
	ERRCHECK(FMOD::Studio::System::create(&m_studioSystem));
	ERRCHECK(m_studioSystem->getCoreSystem(&m_coreSystem));
	ERRCHECK(m_coreSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE_STEREO, 0));
	// Do 3D settings later
	ERRCHECK(m_studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));

	// Add channel groups
	FMOD::ChannelGroup* master, * sfx, * music;
	ERRCHECK(m_coreSystem->getMasterChannelGroup(&master));
	ERRCHECK(m_coreSystem->createChannelGroup("SFX", &sfx));
	ERRCHECK(m_coreSystem->createChannelGroup("Music", &music));
	m_channelGroups[(uint32_t)ChannelGroupID::Master] = master;
	m_channelGroups[(uint32_t)ChannelGroupID::SFX] = sfx;
	m_channelGroups[(uint32_t)ChannelGroupID::Music] = music;

	SPDLOG_INFO("Finished initializing the audio system.");
	return true;
}
void AudioSystem::OnUpdate(const float deltaTime)
{
	ERRCHECK(m_studioSystem->update());
}
void AudioSystem::Shutdown()
{
	ERRCHECK(m_studioSystem->unloadAll());
	ERRCHECK(m_studioSystem->release());
}

UUIDv4::UUID AudioSystem::LoadAudio(const char* audioFilePath)
{
	UUIDv4::UUID uuid;
	if (!m_soundStringToUUID.contains(audioFilePath)) {
		FMOD::Sound* sound;
		ERRCHECK(m_coreSystem->createSound(audioFilePath, FMOD_DEFAULT, nullptr, &sound));
		uuid = m_uuidGenerator.getUUID();
		m_soundStringToUUID[audioFilePath] = uuid;
		m_soundUUIDToFMOD[uuid] = sound;
		return uuid;
	} else {
		SPDLOG_WARN("The audio file at \"{}\" is already loaded!", audioFilePath);
		uuid = m_soundStringToUUID[audioFilePath];
	}
	return uuid;
}
void AudioSystem::PlayAudio(UUIDv4::UUID soundID, ChannelGroupID channelGroup)
{
	FMOD::Sound* sound = m_soundUUIDToFMOD[soundID];
	if (!sound) {
		SPDLOG_ERROR("Cannot play audio that has not been loaded!");
		return;
	}

	// TODO: Save channel in the future?
	FMOD::Channel* channel;
	ERRCHECK(m_coreSystem->playSound(sound, m_channelGroups[(uint32_t)channelGroup], false, &channel));
}
void AudioSystem::UnloadAudio(const char* audioFilePath)
{
	auto uuidIt = m_soundStringToUUID.find(audioFilePath);
	if (uuidIt != m_soundStringToUUID.end()) {
		UUIDv4::UUID uuid = uuidIt->second;
		auto soundIt = m_soundUUIDToFMOD.find(uuid);
		if (soundIt != m_soundUUIDToFMOD.end()) {
			soundIt->second->release();
			m_soundUUIDToFMOD.erase(soundIt);
		}
		m_soundStringToUUID.erase(uuidIt);
	} else {
		SPDLOG_WARN("The audio file at \"{}\" is not loaded!", audioFilePath);
	}
}
}