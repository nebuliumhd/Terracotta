#pragma once

#include <unordered_map>
#include <filesystem>
#include <initializer_list>
#include <utility>
#include <variant>
// Use this #define so that we can statically link EXT
#define AL_ALEXT_PROTOTYPES
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx.h"
#include "AL/efx-presets.h"
#include "glm/glm.hpp"

#include "Subsystem.hpp"

namespace TerracottaEngine
{
constexpr uint32_t INVALID_HANDLE = 0;

using Filepath = std::filesystem::path;
using EffectParamValue = std::variant<ALfloat, ALint>;
using AudioBufferHandle = uint32_t;
using AudioSourceHandle = uint32_t;
using AudioEffectHandle = uint32_t;
using AudioEffectSlotHandle = uint32_t;

enum class AudioType : uint8_t
{
	SFX,
	Music,
	Ambience,
	UI,
	Other
};

struct AudioBuffer
{
	ALuint BufferID;
	std::string Filepath; // Debug name
	ALenum Format;
	ALsizei SampleRate;
	float Duration;
};

struct AudioSource
{
	ALuint SourceID;
	AudioBuffer* Buffer;
	AudioType Type;
	uint8_t Flags;
	// Bit 0 = AL_PITCH
	// Bit 1 = AL_GAIN
	// Bit 2 = AL_LOOPING
};

struct AudioEffect
{
	ALuint EffectID, SlotID;
	std::string EffectName; // Debug name
	ALenum EffectType;
	std::unordered_map<ALenum, std::variant<ALfloat, ALint>> Params;
};

struct AudioEffectSlot
{
	ALuint SlotID;
	std::string Name; // Debug name
	AudioEffect* Effect;
	std::vector<AudioSource*> ConnectedSources;

	bool IsActive() const { return Effect != nullptr; }
	size_t GetSourceCount() const { return ConnectedSources.size(); }
};

struct AudioSourceInfo
{
	uint32_t Index;
	std::vector<AudioSource>* Channel;
};

class AudioSystem : public Subsystem
{
public:
	AudioSystem(SubsystemManager& manager);
	~AudioSystem();

	bool Init() override;
	void OnUpdate(const float deltaTime) override;
	void Shutdown() override;

	AudioBufferHandle LoadAudioToBuffer(const Filepath& oggPath);
	AudioBufferHandle GetBufferHandle(const Filepath& oggPath);
	AudioSourceHandle LoadAudioToBufferAsSource(const Filepath& oggPath, AudioType type);
	void SetBufferToSource(AudioBufferHandle buffer, AudioSourceHandle source);
	void UnloadAudio(AudioBufferHandle buffer);

	AudioSourceHandle CreateSource(AudioBufferHandle buffer, AudioType type);
	void DeleteSource(AudioSourceHandle source);
	AudioEffectHandle CreateEffect(ALint effectType);
	void DeleteEffect(AudioEffectHandle effect);
	AudioEffectSlotHandle CreateEffectSlot();
	void DeleteEffectSlot(AudioEffectSlotHandle slot);

	void ChangeEffectSetting(AudioEffectHandle effect, ALenum type, ALfloat value);
	void ChangeEffectSetting(AudioEffectHandle effect, ALenum type, ALint value);
	// Works irregardless of float or integer types
	void SetEffectSettings(AudioEffectHandle effect, const std::initializer_list<std::pair<ALenum, EffectParamValue>>& config);
	void BindEffectToSlotWithSource(AudioEffectHandle effect, AudioEffectSlotHandle slot, AudioSourceHandle source);
	
	void PlayAudio(AudioSourceHandle source);
	void PlayAudio(const Filepath& oggPath, AudioType type = AudioType::SFX);
	void PauseAudio(AudioSourceHandle source);
	void StopAudio(AudioSourceHandle source);

	// Audio settings
	// void ApplyEffectToSource(ALuint source, ALenum effect);
private:
	ALCcontext* m_alcContext = nullptr;
	ALCdevice* m_alcDevice = nullptr;
	uint32_t m_nextAudioId = 1;
	
	std::unordered_map<std::string, AudioBuffer> m_audioBuffers;
	std::unordered_map<AudioEffectHandle, AudioEffect> m_audioEffects;
	std::unordered_map<AudioEffectSlotHandle, AudioEffectSlot> m_audioEffectSlots;

	// Music source management
	std::vector<AudioSourceInfo> m_sourceIDToChannel; // Doesn't matter if there are duplicate indices since they go to different std::vectors
	float m_cleanupTimer = 0.0f;
	const float m_cleanupInterval = 5.0f;

	// Channels (Swapback arrays)
	std::vector<AudioSource> m_sfxChannel;
	std::vector<AudioSource> m_musicChannel;
	std::vector<AudioSource> m_ambienceChannel;
	std::vector<AudioSource> m_uiChannel;
	std::vector<AudioSource> m_otherChannel;

	AudioBuffer* getBufferObject(AudioBufferHandle buffer);
	bool checkALError(const std::string& context);
};
} // namespace TerracottaEngine