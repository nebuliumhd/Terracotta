#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "InputSystem.hpp"
#include "CameraSystem.hpp"

namespace TerracottaEngine
{
Camera::Camera(SubsystemManager& subsystem) :
	m_managerRef(subsystem)
{
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	View = glm::lookAt(Position, Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Projection = glm::mat4(1.0f);
}
Camera::~Camera()
{

}

void Camera::ProcessInput()
{
	InputSystem* is = m_managerRef.GetSubsystem<InputSystem>();
	if (!is) {
		SPDLOG_ERROR("The InputSystem is not initialized for the camera!");
	}

	if (is->IsKeyDown(GLFW_KEY_W)) {
		Position.y += 0.1f;
		NeedsUpdate = true;
	} else if (is->IsKeyDown(GLFW_KEY_S)) {
		Position.y -= 0.1f;
		NeedsUpdate = true;
	}

	if (is->IsKeyDown(GLFW_KEY_A)) {
		Position.x -= 0.1f;
		NeedsUpdate = true;
	} else if (is->IsKeyDown(GLFW_KEY_D)) {
		Position.x += 0.1f;
		NeedsUpdate = true;
	}
}
void Camera::Update(const float deltaTime)
{
	View = glm::lookAt(Position, Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
}