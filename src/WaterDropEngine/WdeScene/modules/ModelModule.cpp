#include "ModelModule.hpp"

namespace wde::scene {
	void ModelModule::initialize() {
		// Initialize material
		_material->initialize(_gameObject.getDescriptor());
	}

	void ModelModule::render(CommandBuffer& commandBuffer, RenderStage stage) {
		WDE_PROFILE_FUNCTION();
		if (stage != _material->getStage()) // Not in the right rendering stage
			return;

		// Bind material to the game object and push camera constants to it
		_material->bind(commandBuffer);
		_material->pushConstants();
		_material->pushDescriptors();

		// Render model
		_model->bind(commandBuffer);
		_model->render();
	}

	void ModelModule::renderGUI() {
		// Model infos
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("  Model data");
		ImGui::Dummy(ImVec2(0.0f, 0.3f));
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Text(("Vertices count : " + std::to_string(_model->getVertices().size()) + ".").c_str());
		ImGui::Text(("Indices count : " + std::to_string(_model->getIndices().size()) + ".").c_str());


		// Material infos
		ImGui::Dummy(ImVec2(0.0f, 12.0f));
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text("  Material data");
		ImGui::Dummy(ImVec2(0.0f, 0.3f));
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		_material->renderGUI();
	}

	void ModelModule::cleanUp() {
		// Clean up model
		_model->cleanUp();
	}


	// Serializers
	json ModelModule::serialize() {
		return json({
			{"model", _model->serialize()},
			{"material", _material->serialize()}
		});
	}
}
