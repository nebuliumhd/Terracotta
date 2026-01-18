#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "spdlog/spdlog.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "InputSystem.hpp"
#include "CameraSystem.hpp"

namespace TerracottaEngine
{
Camera::Camera(SubsystemManager& subsystem, Window& window) :
	m_managerRef(subsystem), m_window(&window)
{
	glm::ivec2 windowDimensions = window.GetWindowSize();
	m_aspect = (float)windowDimensions.x / windowDimensions.y;

	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	View = glm::lookAt(Position, Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	updateProjection();

	registerCallbacks();
}
Camera::~Camera()
{

}

// CameraSystem.cpp - Smooth camera movement without snapping
void Camera::Update(const float deltaTime)
{
	InputSystem* is = m_managerRef.GetSubsystem<InputSystem>();
	if (!is) {
		SPDLOG_ERROR("The InputSystem is not initialized for the camera!");
	}

	float moveSpeed = 0.1f;

	// Smooth movement (no snapping)
	if (is->IsKeyDown(GLFW_KEY_W)) {
		Position.y += moveSpeed;
		NeedsUpdate = true;
	} else if (is->IsKeyDown(GLFW_KEY_S)) {
		Position.y -= moveSpeed;
		NeedsUpdate = true;
	}
	if (is->IsKeyDown(GLFW_KEY_A)) {
		Position.x -= moveSpeed;
		NeedsUpdate = true;
	} else if (is->IsKeyDown(GLFW_KEY_D)) {
		Position.x += moveSpeed;
		NeedsUpdate = true;
	}

	if (is->IsKeyStartPress(GLFW_KEY_Q) && m_currentZoomLevel < ZOOM_LEVEL_COUNT - 1) {
		m_currentZoomLevel++;
		m_zoom = getZoomLevel(m_currentZoomLevel);
		updateProjection();
		NeedsUpdate = true;
	} else if (is->IsKeyStartPress(GLFW_KEY_E) && m_currentZoomLevel > 0) {
		m_currentZoomLevel--;
		m_zoom = getZoomLevel(m_currentZoomLevel);
		updateProjection();
		NeedsUpdate = true;
	}

	// Use actual position for smooth camera movement
	View = glm::lookAt(Position, Position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::registerCallbacks()
{
	EventSystem* es = m_managerRef.GetSubsystem<EventSystem>();
	if (!es) {
		SPDLOG_ERROR("The EventSystem is not initialized for the camera!");
	}

	// TODO: Force a certain aspect ratio later in the settings
	es->AddListener<WindowSizeEvent>([this](const WindowSizeEvent& e)
	{
		m_aspect = (float)e.Width / e.Height;
		updateProjection();
		NeedsUpdate = true;
	});
}

void Camera::updateProjection()
{
	float zoomedHeight = m_tilesInHeight * m_zoom;
	float zoomedWidth = zoomedHeight * m_aspect;
	Projection = glm::ortho(0.0f, zoomedWidth, 0.0f, zoomedHeight, -1.0f, 1.0f);
}
} // namespace TerracottaEngine