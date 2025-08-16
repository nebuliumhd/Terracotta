#pragma once

#include "glm/glm.hpp"
#include "Subsystem.hpp"

namespace TerracottaEngine
{
class Camera
{
public:
	Camera(SubsystemManager& manager);
	~Camera();

	bool NeedsUpdate = true;
	glm::vec3 Position;
	glm::mat4 View;
	glm::mat4 Projection;

	void ProcessInput();
	void Update(const float deltaTime);

	// Other stuff later...
private:
	static constexpr float MAX_ZOOM = 1.5f;
	static constexpr float MIN_ZOOM = 0.5f;
	float m_zoom = 1.0f;
	float m_moveSpeed = 1.0f;

	SubsystemManager& m_managerRef;
};
}