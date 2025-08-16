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
}