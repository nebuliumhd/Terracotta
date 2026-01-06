#pragma once

#include <algorithm>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
// #include "Application.hpp" Avoid using this header
#include "Window.hpp"

namespace TerracottaEngine
{
class Application;
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
	SubsystemManager(Application& app);
	~SubsystemManager();

	// Make sure you do NOT pass RAW pointers!!!
	template <typename T>
	void RegisterSubsystem(T* subsystem)
	{
		static_assert(std::is_base_of<Subsystem, T>::value, "T must be derived from the Subsystem class!");
		m_subsystemsMap[typeid(T)] = subsystem;
		m_subsystemOrder.push_back(subsystem);
		subsystem->Init();
	}

	template <typename T>
	void UnregisterSubsystem(T* subsystem)
	{
		static_assert(std::is_base_of<Subsystem, T>::value, "T must be derived from the Subsystem class!");
		m_subsystemsMap.erase(typeid(T));

		auto it = std::find(m_subsystemOrder.begin(), m_subsystemOrder.end(), subsystem);
		if (it != m_subsystemOrder.end()) {
			it.second->Shutdown();
			m_subsystemOrder.erase(it);
		}
	}

	template <typename T>
	T* GetSubsystem()
	{
		auto it = m_subsystemsMap.find(typeid(T));
		if (it != m_subsystemsMap.end()) {
			return static_cast<T*>(it->second);
		}
		return nullptr;
	}

	const std::vector<Subsystem*>& GetAllSubsystems() const { return m_subsystemOrder; }
private:
	// Input, Audio, etc.
	std::vector<Subsystem*> m_subsystemOrder;
	std::unordered_map<std::type_index, Subsystem*> m_subsystemsMap;
	Application& m_app;
};
}