#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <functional>
#include "spdlog/spdlog.h"
#include "Subsystem.hpp"

// Template metaprogramming
#define CONSTRUCT_EVENT_TEMPLATE(EventStruct, EnumType, EnumValue, EventVector) \
template <> struct EventTypeTrait<EventStruct>                                  \
	{                                                                           \
		static constexpr EnumType Type = EnumType::EnumValue;                   \
	};                                                                          \
    template <> inline auto& EventSystem::getVector<EventStruct>()              \
	{                                                                           \
		return EventVector;                                                     \
	}

namespace TerracottaEngine
{
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

// All of these struct will contain the data for the receiving function to use
//struct InputEvent
//{
//	int InputCode, Mods;
//	InputEventType Action;
//};
//struct KeyInputEvent : InputEvent
//{
//	int Scancode;
//};
//struct MouseInputEvent : InputEvent
//{};

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
	void AddListener(EventFunc<T> func)
	{
		constexpr uint32_t index = static_cast<uint32_t>(EventTypeTrait<T>::Type);
		auto& listenerVector = getVector<T>()[index];
		listenerVector.push_back([func](const Event& e) {
			func(static_cast<const T&>(e));
		});
	}

	template <typename T>
	void DispatchEvent(const T& event)
	{
		constexpr uint32_t index = static_cast<uint32_t>(EventTypeTrait<T>::Type);
		auto& listenerVector = getVector<T>()[index];
		for (auto& listener : listenerVector) {
			listener(event);
		}
	}
private:
	std::array<std::vector<InputEventFunc>, static_cast<size_t>(InputEventType::NUM_INPUT_EVENTS)> m_inputSubscribers;

	template <typename T>
	auto& getVector()
	{
		SPDLOG_ERROR("There is no event std::vector hooked up to this");
		return nullptr;
	}
};

// TODO: Update these as more get added!
CONSTRUCT_EVENT_TEMPLATE(KeyReleaseEvent, InputEventType, KeyRelease, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(KeyPressEvent, InputEventType, KeyPress, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(KeyRepeatEvent, InputEventType, KeyRepeat, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(MouseButtonReleaseEvent, InputEventType, MouseButtonRelease, m_inputSubscribers);
CONSTRUCT_EVENT_TEMPLATE(MouseButtonPressEvent, InputEventType, MouseButtonPress, m_inputSubscribers);
}