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

	// Need to supply enum with value and end with a null terminator
	ALCint attribs[] = {ALC_MONO_SOURCES, 512, ALC_STEREO_SOURCES, 128, 0};
	m_alcContext = alcCreateContext(m_alcDevice, attribs);
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

	 ALCint monoSources = 0, stereoSources = 0;
	alcGetIntegerv(m_alcDevice, ALC_MONO_SOURCES, 1, &monoSources);
	alcGetIntegerv(m_alcDevice, ALC_STEREO_SOURCES, 1, &stereoSources);

	SPDLOG_INFO("OpenAL Source Limits - Mono: {}, Stereo: {}, Total: {}", monoSources, stereoSources, monoSources + stereoSources);

	// TODO / WARNING: This might break in the future!!!
	m_sourceIDToChannel.reserve(100);

	SPDLOG_INFO("OpenAL Settings - Vendor: {} - Renderer: {} - Version: {}", alGetString(AL_VENDOR), alGetString(AL_RENDERER), alGetString(AL_VERSION));
	SPDLOG_INFO("AudioSystem initialization complete.");
	return true;
}
void AudioSystem::OnUpdate(const float deltaTime)
{
	m_cleanupTimer += deltaTime;
	if (m_cleanupTimer < m_cleanupInterval)
		return;

	// Reset timer and process channels
	m_cleanupTimer = 0.0f;

	auto processChannel = [this](std::vector<AudioSource>& channel) {
		for (size_t i = 0; i < channel.size();) {
			AudioSource& source = channel[i];
			bool isLooping = (source.Flags & (1 << 2)) != 0;

			if (!isLooping) {
				ALint state;
				alGetSourcei(source.SourceID, AL_SOURCE_STATE, &state);

				if (state == AL_STOPPED) {
					SPDLOG_INFO("Autoremoving the finished source: {}", source.SourceID);

					uint32_t lastChannelIndex = static_cast<uint32_t>(channel.size() - 1);
					if (i != lastChannelIndex) {
						std::swap(channel[i], channel[lastChannelIndex]);
						ALuint swappedSourceID = channel[i].SourceID;
						m_sourceIDToChannel[swappedSourceID].Index = static_cast<uint32_t>(i);
					}

					ALuint sourceToDelete = channel[lastChannelIndex].SourceID;
					channel.pop_back();

					if (sourceToDelete < m_sourceIDToChannel.size()) {
						m_sourceIDToChannel[sourceToDelete].Channel = nullptr;
						m_sourceIDToChannel[sourceToDelete].Index = INVALID_HANDLE;
					}

					alDeleteSources(1, &sourceToDelete);
					continue; // Have to check same index because of swap
				}
			}

			i++; // If current source isn't looping, move to next one
		}
	};

	processChannel(m_sfxChannel);
	processChannel(m_musicChannel);
	processChannel(m_ambienceChannel);
	processChannel(m_uiChannel);
	processChannel(m_otherChannel);
}
void AudioSystem::Shutdown()
{
	// Add this before cleaning up buffers:
	for (auto& source : m_sfxChannel) {
		alDeleteSources(1, &source.SourceID);
	}
	for (auto& source : m_musicChannel) {
		alDeleteSources(1, &source.SourceID);
	}
	for (auto& source : m_ambienceChannel) {
		alDeleteSources(1, &source.SourceID);
	}
	for (auto& source : m_uiChannel) {
		alDeleteSources(1, &source.SourceID);
	}
	for (auto& source : m_otherChannel) {
		alDeleteSources(1, &source.SourceID);
	}

	for (auto& pair : m_audioEffects) {
		alDeleteEffects(1, &pair.second.EffectID);
	}
	m_audioEffects.clear();

	for (auto& pair : m_audioEffectSlots) {
		alDeleteAuxiliaryEffectSlots(1, &pair.second.SlotID);
	}
	m_audioEffectSlots.clear();

	for (auto& pair : m_audioBuffers) {
		alDeleteBuffers(1, &pair.second.BufferID);
	}
	m_audioBuffers.clear();

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
	switch (error) {
	case AL_INVALID_NAME:      errorMsg = "AL_INVALID_NAME"; break;
	case AL_INVALID_ENUM:      errorMsg = "AL_INVALID_ENUM"; break;
	case AL_INVALID_VALUE:     errorMsg = "AL_INVALID_VALUE"; break;
	case AL_INVALID_OPERATION: errorMsg = "AL_INVALID_OPERATION"; break;
	case AL_OUT_OF_MEMORY:     errorMsg = "AL_OUT_OF_MEMORY"; break;
	}

	SPDLOG_ERROR("OpenAL error in {}: {}", context, errorMsg);
	return true;
}

AudioBuffer* AudioSystem::getBufferObject(AudioBufferHandle buffer)
{
	for (auto it = m_audioBuffers.begin(); it != m_audioBuffers.end(); it++) {
		if (it->second.BufferID == buffer)
			return &it->second;
	}
	return nullptr;
}

AudioBufferHandle AudioSystem::LoadAudioToBuffer(const Filepath& oggPath)
{
	AudioBuffer buffer = {};
	std::string pathStr = oggPath.string();
	buffer.Filepath = pathStr;

	// Already loaded
	if (AudioBufferHandle loadedHandle = GetBufferHandle(oggPath))
		return loadedHandle;
	
	if (!std::filesystem::exists(oggPath)) {
		SPDLOG_ERROR("File does not exist: \"{}\"", pathStr);
		return INVALID_HANDLE;
	}

	int channels, sampleRate;
	short* sampleData;
	int totalSamples = stb_vorbis_decode_filename(pathStr.c_str(), &channels, &sampleRate, &sampleData);
	if (totalSamples < 0) {
		SPDLOG_ERROR("Failed to decode .ogg file \"{}\"", pathStr);
		return INVALID_HANDLE;
	}
	float duration = (float)totalSamples / sampleRate;
	buffer.SampleRate = sampleRate;
	buffer.Duration = duration;

	switch (channels) {
	case 1:
		buffer.Format = AL_FORMAT_MONO16;
		break;
	case 2:
		buffer.Format = AL_FORMAT_STEREO16;
		break;
	default:
		SPDLOG_ERROR("Unsupported channel count: {}", channels);
		free(sampleData);
		return INVALID_HANDLE;
	}

	alGenBuffers(1, &buffer.BufferID);
	if (checkALError("alGenBuffers")) {
		SPDLOG_ERROR("Failed to generate OpenAL audio buffer.");
		free(sampleData);
		return INVALID_HANDLE;
	}

	alBufferData(buffer.BufferID, buffer.Format, sampleData, totalSamples * channels * sizeof(short), sampleRate);
	free(sampleData);
	if (checkALError("alBufferData")) {
		SPDLOG_ERROR("Failed to load data to OpenAL audio buffer.");
		alDeleteBuffers(1, &buffer.BufferID);
		return INVALID_HANDLE;
	}

	m_audioBuffers[oggPath.filename().string()] = buffer;
	
	SPDLOG_INFO("Loaded .ogg file: \"{}\", channels: {}, sample rate: {}, total samples: {}, duration: {}",
		pathStr, channels, sampleRate, totalSamples, duration);
	return buffer.BufferID;
}
AudioBufferHandle AudioSystem::GetBufferHandle(const Filepath& oggPath)
{
	std::string fileStr = oggPath.filename().string();
	auto it = m_audioBuffers.find(fileStr);
	if (it != m_audioBuffers.end()) {
		return it->second.BufferID;
	}
	
	return INVALID_HANDLE;
}
AudioSourceHandle AudioSystem::LoadAudioToBufferAsSource(const Filepath& oggPath, AudioType type)
{
	AudioBufferHandle buffer = LoadAudioToBuffer(oggPath);
	if (buffer)
		return CreateSource(buffer, type);
	else
		return INVALID_HANDLE;
}
AudioSourceHandle AudioSystem::CreateSource(AudioBufferHandle buffer, AudioType type)
{
	AudioSource source = {};

	if (!buffer) {
		SPDLOG_ERROR("Cannot create an audio source with an invalid buffer.");
		return INVALID_HANDLE;
	}

	AudioBuffer* bufferObj = getBufferObject(buffer);
	if (!bufferObj) {
		SPDLOG_ERROR("Used a bad AudioBufferHandle to generate an AudioSource.");
		return INVALID_HANDLE;
	}
	source.Buffer = bufferObj;
	source.Type = type;

	alGenSources(1, &source.SourceID);
	if (checkALError("CreateSource")) {
		return INVALID_HANDLE;
	}

	SPDLOG_INFO("Created source with ID: {} (current vector size: {})", source.SourceID, m_sourceIDToChannel.size());

	// Attach buffer to source (IMPORTANT!!!)
	alSourcei(source.SourceID, AL_BUFFER, buffer);
	// Set default source properties
	alSourcef(source.SourceID, AL_PITCH, 1.0f);
	alSourcef(source.SourceID, AL_GAIN, 1.0f);
	alSource3f(source.SourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(source.SourceID, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSourcei(source.SourceID, AL_LOOPING, AL_FALSE);

	if (checkALError("CreateSourceWithSettings")) {
		alDeleteSources(1, &source.SourceID);
		return INVALID_HANDLE;
	}

	// IMPORTANT: Resize the array to make sure that it will be able to hold all elements
	if (source.SourceID >= m_sourceIDToChannel.size()) {
		m_sourceIDToChannel.resize(source.SourceID + 1);
	}

	// TODO: Get settings for each of the different channels (have a global config or something)
	uint32_t lastIndex;
	switch (type) {
	case AudioType::SFX:
		m_sfxChannel.push_back(source);
		lastIndex = static_cast<uint32_t>(m_sfxChannel.size() - 1);
		m_sourceIDToChannel[source.SourceID] = {lastIndex, &m_sfxChannel};
		break;
	case AudioType::Music:
		m_musicChannel.push_back(source);
		lastIndex = static_cast<uint32_t>(m_musicChannel.size() - 1);
		m_sourceIDToChannel[source.SourceID] = {lastIndex, &m_musicChannel};
		break;
	case AudioType::Ambience:
		m_ambienceChannel.push_back(source);
		lastIndex = static_cast<uint32_t>(m_ambienceChannel.size() - 1);
		m_sourceIDToChannel[source.SourceID] = {lastIndex, &m_ambienceChannel};
		break;
	case AudioType::UI:
		m_uiChannel.push_back(source);
		lastIndex = static_cast<uint32_t>(m_uiChannel.size() - 1);
		m_sourceIDToChannel[source.SourceID] = {lastIndex, &m_uiChannel};
		break;
	default:
	case AudioType::Other:
		m_otherChannel.push_back(source);
		lastIndex = static_cast<uint32_t>(m_otherChannel.size() - 1);
		m_sourceIDToChannel[source.SourceID] = {lastIndex, &m_otherChannel};
		break;
	}

	return source.SourceID;
}
void AudioSystem::SetBufferToSource(AudioBufferHandle buffer, AudioSourceHandle source)
{
	alSourcei(source, AL_BUFFER, buffer);
	if (!checkALError("SetBufferToSource")) {
		// source. = &buffer;
	} else {
		SPDLOG_ERROR("Failed to assign buffer #{} to source #{}", buffer, source);
	}
}
void AudioSystem::UnloadAudio(AudioBufferHandle buffer)
{
	if (!buffer)
        return;

    // Find and remove from cache
    for (auto it = m_audioBuffers.begin(); it != m_audioBuffers.end(); it++) {
        if (it->second.BufferID == buffer) {
            m_audioBuffers.erase(it);
            SPDLOG_INFO("Unloaded audio buffer: {}", buffer);
			alDeleteBuffers(1, &buffer);
			checkALError("UnloadAudio");
            return;
        }
    }

	SPDLOG_ERROR("Unable to find buffer #{}", buffer);
}
void AudioSystem::DeleteSource(AudioSourceHandle source)
{
	if (!source || source >= m_sourceIDToChannel.size()) {
		SPDLOG_ERROR("Unable to delete invalid AudioSource: {}", source);
		return;
	}

	AudioSourceInfo& info = m_sourceIDToChannel[source];
	if (!info.Channel) {
		SPDLOG_ERROR("AudioSource {} already deleted or doesn't exist.", source);
		return;
	}

	uint32_t indexToRemove = info.Index;
	std::vector<AudioSource>& channel = *info.Channel;
	if (indexToRemove >= channel.size() || channel[indexToRemove].SourceID != source) {
		SPDLOG_ERROR("The AudioSource {} is not at the specified channel.", source);
		return;
	}

	uint32_t lastIndex = static_cast<uint32_t>(channel.size() - 1);
	if (indexToRemove != lastIndex) {
		std::swap(channel[indexToRemove], channel[lastIndex]);
		// Update the swapped element's index!!!
		ALuint swappedSourceID = channel[indexToRemove].SourceID;
		m_sourceIDToChannel[swappedSourceID].Index = indexToRemove;
	}
	channel.pop_back(); // Avoid memory leak

	info.Channel = nullptr;
	info.Index = INVALID_HANDLE;

	alDeleteSources(1, &source);
	checkALError("DeleteSource");
}
AudioEffectHandle AudioSystem::CreateEffect(ALint effectType)
{
	AudioEffect effect;
	alGenEffects(1, &effect.EffectID);
	alEffecti(effect.EffectID, AL_EFFECT_TYPE, effectType);
	if (checkALError("CreateEffect")) {
		SPDLOG_ERROR("Unable to create OpenAL effect.");
		return INVALID_HANDLE;
	}

	m_audioEffects[effect.EffectID] = effect;
	return effect.EffectID;
}
void AudioSystem::DeleteEffect(AudioEffectHandle effect)
{
	if (effect) {
		alDeleteEffects(1, &effect);
		m_audioEffects.erase(effect);
		checkALError("DeleteEffect");
	} else {
		SPDLOG_ERROR("Unable to delete invalid AudioEffect.", effect);
	}
	
}
AudioEffectSlotHandle AudioSystem::CreateEffectSlot()
{
	AudioEffectSlot slot = {};
	alGenAuxiliaryEffectSlots(1, &slot.SlotID);
	if (checkALError("CreateEffectSlot")) {
		return INVALID_HANDLE;
	}

	m_audioEffectSlots[slot.SlotID] = slot;
	return slot.SlotID;
}
void AudioSystem::ChangeEffectSetting(AudioEffectHandle effect, ALenum type, ALfloat value)
{
	if (!effect) {
		SPDLOG_ERROR("Tried to set configurations on an invalid AudioEffect.");
		return;
	}

	m_audioEffects[effect].Params[type] = value;
}
void AudioSystem::ChangeEffectSetting(AudioEffectHandle effect, ALenum type, ALint value)
{
	if (!effect) {
		SPDLOG_ERROR("Tried to set configurations on an invalid AudioEffect.");
		return;
	}

	m_audioEffects[effect].Params[type] = value;
}
void AudioSystem::SetEffectSettings(AudioEffectHandle effect, const std::initializer_list<std::pair<ALenum, EffectParamValue>>& config)
{
	if (!effect) {
		SPDLOG_ERROR("Tried to set configurations on an invalid AudioEffect.");
		return;
	}

	for (const auto& [effectParam, effectValue] : config) {
		if (std::holds_alternative<ALfloat>(effectValue)) {
			alEffectf(effect, effectParam, std::get<ALfloat>(effectValue));
		} else {
			alEffecti(effect, effectParam, std::get<ALint>(effectValue));
		}
	}
}
void AudioSystem::BindEffectToSlotWithSource(AudioEffectHandle effect, AudioEffectSlotHandle slot, AudioSourceHandle source)
{
	if (effect && slot && source) {
		alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect);
		alSource3i(source, AL_AUXILIARY_SEND_FILTER, slot, 0, AL_FILTER_NULL);
		checkALError("BindEffectToSlotWithSource");
	} else {
		SPDLOG_ERROR("One of the following is invalid: AudioEffect, AudioEffectSlot, AudioSource.");
	}
}
void AudioSystem::DeleteEffectSlot(AudioEffectSlotHandle slot)
{
	if (slot) {
		alDeleteAuxiliaryEffectSlots(1, &slot);
		m_audioEffectSlots.erase(slot);
		checkALError("DeleteEffectSlot");
	} else {
		SPDLOG_ERROR("Unable to delete invalid AudioEffectSlot.", slot);
	}
}
void AudioSystem::PlayAudio(AudioSourceHandle source)
{
	if (source) {
		alSourcePlay(source);
		checkALError("PlayAudio");
	} else {
		SPDLOG_ERROR("Cannot play an invalid AudioSource.");
	}
}
void AudioSystem::PlayAudio(const Filepath& oggPath, AudioType type)
{
	AudioBufferHandle buffer = GetBufferHandle(oggPath);
	if (buffer == INVALID_HANDLE)
		buffer = LoadAudioToBuffer(oggPath);
	if (buffer == INVALID_HANDLE) {
		SPDLOG_ERROR("Aborting playing audio at \"{}\"", oggPath.string());
		return;
	}

	AudioSourceHandle source = CreateSource(buffer, type);
	if (source == INVALID_HANDLE) {
		SPDLOG_ERROR("Failed to create source for: {}", oggPath.string());
		return;
	}

	// TODO: Add additional basic parameters if necessary
	PlayAudio(source);
}
void AudioSystem::PauseAudio(AudioSourceHandle source)
{
	if (source) {
		alSourcePause(source);
		checkALError("PauseAudio");
	} else {
		SPDLOG_ERROR("Cannot play an invalid AudioSource.");
	}
}
void AudioSystem::StopAudio(AudioSourceHandle source)
{
	if (source) {
		alSourceStop(source);
		checkALError("StopAudio");
	} else {
		SPDLOG_ERROR("Cannot play an invalid AudioSource.");
	}
}
} // namespace TerracottaEngine