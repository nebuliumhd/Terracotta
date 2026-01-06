#include "spdlog/spdlog.h"
// Warning: Do not add more than one stb_vorbis.c include (if so use the #define above)
#define STB_VORBIS_HEADER_ONLY
#include "stb/stb_vorbis.c"
#include "AudioSystem.hpp"

namespace TerracottaEngine
{
AudioSystem::AudioSystem(SubsystemManager& manager) : Subsystem(manager)
{
	// Init should be called when we register the subsystem
}
AudioSystem::~AudioSystem()
{
	// Shutdown() is automatically called by SubsystemManager
}

bool AudioSystem::Init()
{
	SPDLOG_INFO("Initializing the audio system (OpenAL Soft v1.25.0)...");

	m_alcDevice = alcOpenDevice(nullptr);
	if (!m_alcDevice) {
		SPDLOG_ERROR("Failed find an OpenAL compatible device.");
		return false;
	}

	m_alcContext = alcCreateContext(m_alcDevice, nullptr);
    if (!m_alcContext) {
		SPDLOG_ERROR("Failed to create OpenAL context.");
		alcCloseDevice(m_alcDevice);
        m_alcDevice = nullptr;
        return false;
	}
	
	if (!alcMakeContextCurrent(m_alcContext)) {
        SPDLOG_ERROR("Failed to set OpenAL context.");
        alcDestroyContext(m_alcContext);
        alcCloseDevice(m_alcDevice);
        m_alcContext = nullptr;
        m_alcDevice = nullptr;
        return false;
    }

	// TODO: Check if we have basic DSP

	// Set listener defaults
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    ALfloat listenerOri[] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    alListenerfv(AL_ORIENTATION, listenerOri);

	if (checkALError("Init")) {
        return false;
    }

	if (!alcIsExtensionPresent(m_alcDevice, "ALC_EXT_EFX")) {
		SPDLOG_ERROR("WE DON'T HAVE DSP!!!");
		return false;
	}

	alGenAuxiliaryEffectSlots(1, &m_effectSlot);
	if (checkALError("Create effect slot")) {
		SPDLOG_ERROR("Could not create a DSP effect for OpenAL.");
		return false;
	}

	SPDLOG_INFO("OpenAL Settings - Vendor: {} - Renderer: {} - Version: {}", alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_VERSION));
	SPDLOG_INFO("AudioSystem initialization complete.");
	return true;
}
void AudioSystem::OnUpdate(const float deltaTime)
{
	
}
void AudioSystem::Shutdown()
{
	if (m_effectSlot != 0) {
		alDeleteAuxiliaryEffectSlots(1, &m_effectSlot);
		m_effectSlot = 0;
	}

	for (auto& pair : m_audio) {
		alDeleteBuffers(1, &pair.second);
	}
	m_audio.clear();

	if (m_alcContext) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(m_alcContext);
		m_alcContext = nullptr;
	}
	if (m_alcDevice) {
		alcCloseDevice(m_alcDevice);
		m_alcDevice = nullptr;
	}

	SPDLOG_INFO("AudioSystem shutdown complete.");
}

bool AudioSystem::checkALError(const std::string& context)
{
	ALenum error = alGetError();
	if (error == AL_NO_ERROR)
		return false;

	const char* errorMsg = "Unknown";
	switch (error)
	{
	case AL_INVALID_NAME:      errorMsg = "AL_INVALID_NAME"; break;
	case AL_INVALID_ENUM:      errorMsg = "AL_INVALID_ENUM"; break;
	case AL_INVALID_VALUE:     errorMsg = "AL_INVALID_VALUE"; break;
	case AL_INVALID_OPERATION: errorMsg = "AL_INVALID_OPERATION"; break;
	case AL_OUT_OF_MEMORY:     errorMsg = "AL_OUT_OF_MEMORY"; break;
	}

	SPDLOG_ERROR("OpenAL error in {}: {}", context, errorMsg);
	return true;
}

ALuint AudioSystem::LoadAudio(const std::filesystem::path& oggPath)
{
	std::string pathStr = oggPath.string();
	auto it = m_audio.find(pathStr);
    if (it != m_audio.end()) {
        SPDLOG_WARN("Audio already loaded: \"{}\"", pathStr);
        return it->second;
    }
	
	if (!std::filesystem::exists(oggPath)) {
		SPDLOG_ERROR("File does not exist: \"{}\"", pathStr);
		return 0;
	}


	int channels, sampleRate;
	short* sampleData;
	int totalSamples = stb_vorbis_decode_filename(pathStr.c_str(), &channels, &sampleRate, &sampleData);
	if (totalSamples < 0) {
		SPDLOG_ERROR("Failed to decode .ogg file \"{}\"", pathStr);
		return 0;
	}

	ALenum format;
	if (channels == 1) {
		format = AL_FORMAT_MONO16;
	} else if (channels == 2) {
		format = AL_FORMAT_STEREO16;
	} else {
		SPDLOG_ERROR("Unsupported channel count: {}", channels);
		free(sampleData);
		return 0;
	}

	ALuint buffer = 0;
	alGenBuffers(1, &buffer);
	if (checkALError("alGenBuffers")) {
		SPDLOG_ERROR("Failed to generate OpenAL sound buffer.");
		free(sampleData);
		return 0;
	}

	alBufferData(buffer, format, sampleData, totalSamples * channels * sizeof(short), sampleRate);
	free(sampleData);
	if (checkALError("alBufferData")) {
		SPDLOG_ERROR("Failed to load data to OpenAL sound buffer.");
		alDeleteBuffers(1, &buffer);
		return 0;
	}

	m_audio[pathStr] = buffer;
	
	SPDLOG_INFO("Loaded .ogg file: \"{}\", channels: {}, sample rate: {}, total samples: {}, duration: {}",
		pathStr, channels, sampleRate, totalSamples, (float)totalSamples / sampleRate);
	return buffer;
}
void AudioSystem::UnloadAudio(ALuint buffer)
{
	if (buffer == 0)
        return;

    // Find and remove from cache
    for (auto it = m_audio.begin(); it != m_audio.end(); it++) {
        if (it->second == buffer) {
            alDeleteBuffers(1, &buffer);
            checkALError("UnloadAudio");
            m_audio.erase(it);
            SPDLOG_INFO("Unloaded audio buffer: {}", buffer);
            return;
        }
    }

    alDeleteBuffers(1, &buffer);
    checkALError("UnloadAudio");
}

ALuint AudioSystem::CreateAudioSource(ALuint buffer)
{
	if (buffer == 0) {
        SPDLOG_ERROR("Cannot create an audio source with an invalid buffer.");
        return 0;
    }

    ALuint source;
    alGenSources(1, &source);
    if (checkALError("CreateAudioSource")) {
        return 0;
    }

    // Attach buffer to source (IMPORTANT!!!)
    alSourcei(source, AL_BUFFER, buffer);
    // Set default source properties
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcef(source, AL_GAIN, 1.0f);
    alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    if (checkALError("CreateAudioSource")) {
        alDeleteSources(1, &source);
        return 0;
    }

    return source;
}
void AudioSystem::DestroyAudioSource(ALuint source)
{
	if (source == 0)
        return;

    alDeleteSources(1, &source);
    checkALError("DestroyAudioSource");
}

void AudioSystem::PlayAudio(ALuint source) {
     if (source == 0) {
        SPDLOG_ERROR("Cannot play an invalid audio source.");
        return;
    }

	// Create effect
	// ALuint effect = 0;
	// alGenEffects(1, &effect);
	// alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_PITCH_SHIFTER);
	// alEffectf(effect, AL_REVERB_DECAY_TIME, 2.0f);  // 2 second decay
	// alEffectf(effect, AL_REVERB_DENSITY, 0.8f);      // etc.

	// Apply effect to source
	// alAuxiliaryEffectSloti(m_effectSlot, AL_EFFECTSLOT_EFFECT, effect);
	// alSource3i(source, AL_AUXILIARY_SEND_FILTER, m_effectSlot, 0, AL_FILTER_NULL);

    alSourcePlay(source);
    checkALError("PlayAudio");
}
void AudioSystem::PauseAudio(ALuint source)
{
	if (source == 0) {
        SPDLOG_ERROR("Cannot pause an invalid audio source.");
        return;
    }

    alSourcePause(source);
    checkALError("PauseAudio");
}
void AudioSystem::StopAudio(ALuint source)
{
	if (source == 0) {
        SPDLOG_ERROR("Cannot stop an invalid audio source.");
        return;
    }

    alSourceStop(source);
    checkALError("StopAudio");
}
}