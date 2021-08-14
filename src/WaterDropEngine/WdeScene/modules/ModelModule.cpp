#include "ModelModule.hpp"
#include "../objects/GameObject.hpp"

namespace wde::scene {
	void ModelModule::initialize() {
		// Initialize materials
		for (auto& material : _materials)
			material->initialize(_gameObject.getDescriptor());
	}

	void ModelModule::render(CommandBuffer& commandBuffer, RenderStage stage) {
		WDE_PROFILE_FUNCTION();

		// Render materials
		for (auto& material : _materials) {
			if (stage != material->getStage()) // Not in the right rendering stage
				continue;

			// Bind material to the game object and push camera constants to it
			material->bind(commandBuffer);
			material->pushConstants();
			material->pushDescriptors();

			// Render model
			_model->bind(commandBuffer);
			_model->render();
		}
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

		// Material GUI
		for (auto& material : _materials) {
			material->initialize(_gameObject.getDescriptor());
			ImGui::Separator();
		}
	}

	void ModelModule::cleanUp() {
		// Clean up model
		_model->cleanUp();

		// Clean up materials
		_materials.clear();
	}


	// Serializers
	json ModelModule::serialize() {
		json serializedMaterials = json::array();
		for (auto& material : _materials)
			serializedMaterials.push_back(material->serialize());

		return json({
			{"model", _model->serialize()},
			{"material", serializedMaterials}
		});
	}

	void ModelModule::deserialize(json data) {
		// Deserialize associated model and materials if they have one
		_model->deserialize(data["model"]);

		json serializedMaterials = json::array();
		for (int i = 0; i < data["material"].size(); i++)
			_materials[i]->deserialize(data["material"][i]);
	}
}
