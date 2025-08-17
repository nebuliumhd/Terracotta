#pragma once

#include <random>
#include <array>
#include <string>
#include "fmod_studio.hpp"
#include "fmod.hpp"
#include "endianness.h"
#include "uuid_v4.h"
#include "Subsystem.hpp"

namespace TerracottaEngine
{
enum class ChannelGroupID : uint32_t
{
	Master,
	SFX,
	Music,
	NUM_FMOD_CHANNEL_GROUPS
};

class AudioSystem : public Subsystem
{
public:
	AudioSystem(SubsystemManager& manager);
	~AudioSystem();

	bool Init() override;
	void OnUpdate(const float deltaTime) override;
	void Shutdown() override;

	UUIDv4::UUID LoadAudio(const char* audioFilePath);
	void PlayAudio(UUIDv4::UUID soundID, ChannelGroupID channelGroup);
	// WARNING: MUST STOP ALL INSTANCES OF THIS SOUND BEING PLAYED (STOP SOUNDS AT SPECIFIC CHANNELGROUP(S))
	void UnloadAudio(const char* audioFilePath);

	// WARNING: Assumes that audio has been loaded!!!
	UUIDv4::UUID GetAudioUUID(const char* audioFilePath) const { return m_soundStringToUUID.at(audioFilePath); }
private:
	FMOD::Studio::System* m_studioSystem = nullptr;
	FMOD::System* m_coreSystem = nullptr;
	std::array<FMOD::ChannelGroup*, static_cast<size_t>(ChannelGroupID::NUM_FMOD_CHANNEL_GROUPS)> m_channelGroups;

	UUIDv4::UUIDGenerator<std::mt19937_64> m_uuidGenerator;
	std::unordered_map<std::string, UUIDv4::UUID> m_soundStringToUUID; // For debugging only
	std::unordered_map<UUIDv4::UUID, FMOD::Sound*> m_soundUUIDToFMOD; // Use this at RUN TIME!!!
};
}