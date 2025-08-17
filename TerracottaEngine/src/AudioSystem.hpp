#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"
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

	void PlayAudio(const char* audioFilePath);
private:
	FMOD::Studio::System* m_studioSystem = nullptr;
	FMOD::System* m_coreSystem = nullptr;
};
}