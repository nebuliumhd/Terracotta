#pragma once

#include <unordered_map>
#include <filesystem>
// TODO: Check if we need forward declaration
#include "AL/al.h"
#include "AL/alc.h"

#include "Subsystem.hpp"

namespace TerracottaEngine
{
class AudioSystem : public Subsystem
{
public:
	AudioSystem(SubsystemManager& manager);
	~AudioSystem();

	bool Init() override;
	void OnUpdate(const float deltaTime) override;
	void Shutdown() override;

	ALuint LoadAudio(const std::filesystem::path& oggPath);
	void UnloadAudio(ALuint buffer);
	ALuint CreateAudioSource(ALuint buffer);
	void DestroyAudioSource(ALuint source);
	void PlayAudio(ALuint source);
	void PauseAudio(ALuint source);
	void StopAudio(ALuint source);
private:
	ALCcontext* m_alcContext = nullptr;
	ALCdevice* m_alcDevice = nullptr;
	std::unordered_map<std::string, ALuint> m_audio;
	bool checkALError(const std::string& context);
};
}