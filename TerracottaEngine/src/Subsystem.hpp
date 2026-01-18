#pragma once

#include <algorithm>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <memory>
#include "Window.hpp"

namespace TerracottaEngine
{
class SubsystemManager;

class Subsystem
{
public:
	Subsystem(SubsystemManager& manager) : m_manager(manager) {}
	virtual ~Subsystem() = default;
	virtual bool Init() = 0;
	virtual void OnUpdate(const float deltaTime) = 0;
	virtual void Shutdown() = 0;
protected:
	SubsystemManager& m_manager;
};

// Have the application class carry an instance to manage on its behalf
class SubsystemManager
{
public:
	SubsystemManager();
	~SubsystemManager();

	void Update(float deltaTime);

	// TODO: Possibly add variadic arguments for subsystems that require them
	template <typename T, typename... Args>
	T* RegisterSubsystem(Args&&... args)
	{
		static_assert(std::is_base_of<Subsystem, T>::value, "T must be derived from the Subsystem class!");

		// Create the subsystem with whatever arguments it needs
		auto subsystem = std::make_unique<T>(std::forward<Args>(args)...);
		T* rawSubsystemPtr = subsystem.get();

		// Store and init
		m_subsystemsMap[typeid(T)] = std::move(subsystem);
		m_subsystemOrder.push_back(rawSubsystemPtr);
		rawSubsystemPtr->Init();

		return rawSubsystemPtr;
	}

	template <typename T>
	void UnregisterSubsystem(T* subsystem)
	{
		static_assert(std::is_base_of<Subsystem, T>::value, "T must be derived from the Subsystem class!");

		auto it = m_subsystemsMap.find(typeid(T));
		if (it != m_subsystemsMap.end()) {
			Subsystem* subsystem = it->second.get();

			// Shutdown the subsystem before it's removal
			subsystem->Shutdown();

			// Remove from order vectors
			auto orderIt = std::find(m_subsystemOrder.begin(), m_subsystemOrder.end(), subsystem);
			if (orderIt != m_subsystemOrder.end()) {
				m_subsystemOrder.erase(orderIt);
			}

			// Remove from map
			m_subsystemsMap.erase(it);
		}
	}

	template <typename T>
	T* GetSubsystem()
	{
		auto it = m_subsystemsMap.find(typeid(T));
		if (it != m_subsystemsMap.end()) {
			return static_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	const std::vector<Subsystem*>& GetAllSubsystems() const { return m_subsystemOrder; }
private:
	// Input, Audio, etc.
	std::vector<Subsystem*> m_subsystemOrder;
	std::unordered_map<std::type_index, std::unique_ptr<Subsystem>> m_subsystemsMap;
};
} // namespace TerracottaEngine