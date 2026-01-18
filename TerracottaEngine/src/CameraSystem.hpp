#pragma once

#include "glm/glm.hpp"
#include "Subsystem.hpp"

namespace TerracottaEngine
{
class Camera
{
public:
	Camera(SubsystemManager& manager, Window& window);
	~Camera();

	bool NeedsUpdate = true;
	glm::vec3 Position;
	glm::mat4 View;
	glm::mat4 Projection;

	void Update(const float deltaTime);

	// Other stuff later...
private:
	static constexpr int ZOOM_LEVEL_COUNT = 4;
	int m_currentZoomLevel = 0; // Start at 1.0x zoom
	float m_zoom = 1.0f;
	float m_moveSpeed = 1.0f;
	float m_aspect;
	float m_tilesInHeight = 32.0f;

	SubsystemManager& m_managerRef;
	Window* m_window;

	void updateProjection();
	void registerCallbacks();

	float getZoomLevel(int level) const
	{
		// Only integer zooms for pixel-perfect rendering
		static const float ZOOM_LEVELS[] = {1.0f, 2.0f, 3.0f, 4.0f};
		return ZOOM_LEVELS[level];
	}
};
} // namespace TerracottaEngine