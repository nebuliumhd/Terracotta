#include "Subsystem.hpp"

namespace TerracottaEngine
{
SubsystemManager::SubsystemManager(Application& app) :
	m_app(app)
{

}
SubsystemManager::~SubsystemManager()
{
	/*for (Subsystem* subsystem : m_subsystemOrder) {
		delete subsystem;
	}*/
}
}