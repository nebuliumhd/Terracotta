#include "Subsystem.hpp"

namespace TerracottaEngine
{
SubsystemManager::SubsystemManager()
{
    // Should call Init() AS each subsystem is added
}
SubsystemManager::~SubsystemManager()
{
    // Shutdown in the OPPOSITE order for safety reasons
	for (auto it = m_subsystemOrder.rbegin(); it != m_subsystemOrder.rend(); it++) {
        (*it)->Shutdown();
    }
}

void SubsystemManager::Update(float deltaTime)
{
	for (Subsystem* subsystem : m_subsystemOrder) {
		subsystem->OnUpdate(deltaTime);
	}
}
} // namespace TerracottaEngine