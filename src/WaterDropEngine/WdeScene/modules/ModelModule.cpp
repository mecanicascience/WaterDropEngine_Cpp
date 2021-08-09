#include "ModelModule.hpp"
#include "../WdeSceneManager.hpp"

namespace wde::scene {
	void ModelModule::render(CommandBuffer& commandBuffer, RenderStage stage) {
		WDE_PROFILE_FUNCTION();
		if (stage != _material->getStage()) // Not in the right rendering stage
			return;

		// Bind material to the game object and push camera constants to it
		_material->bind(commandBuffer);
		_material->pushConstants(_gameObject, WdeSceneManager::get().getActiveScene()->getCamera());

		// Render model
		_model->bind(commandBuffer);
		_model->render();
	}

	void ModelModule::renderGUI() {
		// Model infos
		ImGui::Text("Object model data.");
		ImGui::Text(("Vertices count : " + std::to_string(_model->getVertices().size()) + ".").c_str());
		ImGui::Text(("Indices count : " + std::to_string(_model->getIndices().size()) + ".").c_str());

		// Material infos
		ImGui::Text("Material data.");
		ImGui::Text(("Vertex shader : " + _material->getVertexShader() + ".").c_str());
		ImGui::Text(("Fragment shader : " + _material->getFragmentShader() + ".").c_str());

		std::string polygonMode;
		if (_material->getPolygonMode() == VK_POLYGON_MODE_POINT)
			polygonMode = "Point mode";
		else if (_material->getPolygonMode() == VK_POLYGON_MODE_LINE)
			polygonMode = "Line mode";
		else
			polygonMode = "Fill mode";
		ImGui::Text(("Polygon mode : " + polygonMode + ".").c_str());
	}

	void ModelModule::cleanUp() {
		// Clean up model
		_model->cleanUp();
	}
}
