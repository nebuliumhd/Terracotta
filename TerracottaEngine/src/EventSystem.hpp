#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include <algorithm>
#include "spdlog/spdlog.h"
#include "Subsystem.hpp"

// Template metaprogramming
#define CONSTRUCT_EVENT_TEMPLATE(EventStruct, EnumType, EnumValue, EventVector) \
template <> struct EventTypeTrait<EventStruct>                                  \
	{                                                                           \
		static constexpr EnumType Type = EnumType::EnumValue;                   \
	};                                                                          \
    template <> inline auto& EventSystem::getArray<EventStruct>()               \
	{                                                                           \
		return EventVector;                                                     \
	}

namespace TerracottaEngine
{
// Forward declaration
template <typename T> struct EventTypeTrait;

enum class InputEventType : uint32_t
{
	KeyRelease,
	KeyPress,
	KeyRepeat,
	MouseButtonRelease,
	MouseButtonPress,
	NUM_INPUT_EVENTS // Last enum, don't touch/move relative to the other events
};

// Base event struct
struct Event
{};

// Input events
struct InputEvent : public Event
{
	int Keycode, Mods;
};
struct KeyReleaseEvent : public InputEvent
{};
struct KeyPressEvent : public InputEvent
{};
struct KeyRepeatEvent : public InputEvent
{};
struct MouseButtonReleaseEvent : public InputEvent
{};
struct MouseButtonPressEvent : public InputEvent
{};

template <typename T>
using EventFunc = std::function<void(const T&)>;
using BaseEventFunc = std::function<void(const Event&)>;
using InputEventFunc = std::function<void(const InputEvent&)>;

class EventSystem : public Subsystem
{
public:
	EventSystem(SubsystemManager& manager);
	~EventSystem();

	bool Init() override;
	void OnUpdate(const float deltaTime) override;
	void Shutdown() override;

	void LinkToGLFWWindow(GLFWwindow* glfwWindow);

	template <typename T>
	uint64_t AddListener(EventFunc<T> func)
	{
		// We use ::Type since it's a static public member of the struct
		constexpr uint32_t index = static_cast<uint32_t>(EventTypeTrait<T>::Type);
		auto& listenerVector = getArray<T>()[index];
		
		// Generate unique ID for this listener
		uint64_t id = m_nextListenerId++;
		
		// Create entry with ID and wrapped callback
		ListenerEntry entry;
		entry.ID = id;
		entry.Callback = [func](const Event& e) {
			func(static_cast<const T&>(e));
		};
		
		listenerVector.push_back(std::move(entry));
		return id;
	}

	template <typename T>
	void RemoveListener(uint64_t handleID)
	{
		constexpr uint32_t index = static_cast<uint32_t>(EventTypeTrait<T>::Type);
		auto& listenerVector = getArray<T>()[index];

		// SPDLOG_WARN("RemoveListener: Looking for ID {} in vector of size {}", handleID, listenerVector.size());
		
		// Find the listener by ID
		auto it = std::find_if(listenerVector.begin(), listenerVector.end(),
			[handleID](const ListenerEntry& entry) { 
				// SPDLOG_WARN("  Checking entry with ID {}", entry.ID);
				return entry.ID == handleID; 
			});
		
		if (it != listenerVector.end()) {
			// SPDLOG_WARN("\tFound it! Removing...");
			*it = std::move(listenerVector.back());
			listenerVector.pop_back();
			// SPDLOG_WARN("\tRemoved. New size: {}", listenerVector.size());
		} else {
			// SPDLOG_ERROR("\tNOT FOUND!");
		}
	}

	template <typename T>
	void DispatchEvent(const T& event)
	{
		constexpr uint32_t index = static_cast<uint32_t>(EventTypeTrait<T>::Type);
		auto& listenerVector = getArray<T>()[index];
		
		for (auto& entry : listenerVector) {
			entry.Callback(event);
		}
	}

private:
	struct ListenerEntry {
		uint64_t ID;
		InputEventFunc Callback;
	};

	std::array<std::vector<ListenerEntry>, static_cast<size_t>(InputEventType::NUM_INPUT_EVENTS)> m_inputSubscribers;
	uint64_t m_nextListenerId = 1;

	template <typename T>
	auto& getArray()
	{
		static_assert(sizeof(T) == 0, "Event type not registered! Use CONSTRUCT_EVENT_TEMPLATE macro.");
		// This line never executes, but satisfies return type requirement
		return m_inputSubscribers;
	}
};

// TODO: Update these as more get added (they are in the header so they are generated properly)!
CONSTRUCT_EVENT_TEMPLATE(KeyReleaseEvent, InputEventType, KeyRelease, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(KeyPressEvent, InputEventType, KeyPress, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(KeyRepeatEvent, InputEventType, KeyRepeat, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(MouseButtonReleaseEvent, InputEventType, MouseButtonRelease, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(MouseButtonPressEvent, InputEventType, MouseButtonPress, m_inputSubscribers);
}