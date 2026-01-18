#include <algorithm>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "spdlog/spdlog.h"
#include "Layers.hpp"

namespace TerracottaEngine
{
LayerStack::LayerStack()
{

}
LayerStack::~LayerStack()
{
	// TODO: May have to specify a specific order in which layers are deconstructed
}

void LayerStack::PushLayer(Layer* layer)
{
	m_layers.emplace_back(layer);
	layer->OnAttach();
}
void LayerStack::PopLayer(Layer* layer)
{
	auto it = std::find(m_layers.begin(), m_layers.end(), layer);

	// If layer is found
	if (it != m_layers.end()) {
		layer->OnDetach();
		m_layers.erase(it);
	}
}

DearImGuiLayer::DearImGuiLayer(GLFWwindow* glfwWindow, const std::string& layerName) :
	Layer(layerName), m_glfwWindow(glfwWindow)
{

}
DearImGuiLayer::~DearImGuiLayer()
{

}

void DearImGuiLayer::OnAttach()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.ConfigFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if (!m_glfwWindow) {
		SPDLOG_ERROR("Could not find an application window!");
	}

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}
void DearImGuiLayer::OnDetach()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void DearImGuiLayer::OnUpdate(const float deltaTime)
{

}
void DearImGuiLayer::OnRender()
{
	static bool showDemo = true;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(&showDemo);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void DearImGuiLayer::OnImGuiRender()
{

}
} // namespace TerracottaEngine