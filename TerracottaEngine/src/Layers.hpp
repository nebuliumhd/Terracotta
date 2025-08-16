#pragma once

#include <vector>
#include <string>

namespace TerracottaEngine
{
class Layer
{
public:
	Layer(const std::string& name) :
		m_name(name)
		{}
	virtual ~Layer() {}

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(const float deltaTime) {}
	virtual void OnRender() {}
	virtual void OnImGuiRender() {}

	const std::string& GetName() { return m_name; }
private:
	std::string m_name;
};

class LayerStack
{
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PopLayer(Layer* layer);

	std::vector<Layer*>::iterator begin() { return m_layers.begin(); }
	std::vector<Layer*>::iterator end() { return m_layers.end(); }
private:
	std::vector<Layer*> m_layers;
};

class DearImGuiLayer
{
public:
private:
};
}