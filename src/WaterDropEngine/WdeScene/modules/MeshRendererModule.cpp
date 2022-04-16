#include "MeshRendererModule.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::scene {
	MeshRendererModule::MeshRendererModule(GameObject &gameObject) : Module(gameObject, "Mesh Renderer", ICON_FA_GHOST) {}

	MeshRendererModule::MeshRendererModule(GameObject &gameObject, const std::string &data) : Module(gameObject, "Mesh Renderer", ICON_FA_GHOST) {
		WDE_PROFILE_FUNCTION();
		auto dataJ = json::parse(data);
		auto& wde = WaterDropEngine::get();
		_materialName = dataJ["material"].get<std::string>();
		_meshName = dataJ["mesh"].get<std::string>();
		_material = wde.getResourceManager().load<resource::Material>(wde.getInstance().getScene()->getPath() + "data/materials/" + _materialName);
		_mesh = wde.getResourceManager().load<resource::Mesh>(wde.getInstance().getScene()->getPath() + "data/meshes/" + _meshName);
	}

	MeshRendererModule::~MeshRendererModule() {
		WDE_PROFILE_FUNCTION();
		// Release material
		if (_material != nullptr) {
			WaterDropEngine::get().getResourceManager().release(_material->getPath());
			_materialName = "";
			_material = nullptr;
		}

		// Release mesh
		if (_mesh != nullptr) {
			WaterDropEngine::get().getResourceManager().release(_mesh->getPath());
			_meshName = "";
			_mesh = nullptr;
		}
	}


	void MeshRendererModule::drawGUI() {
		WDE_PROFILE_FUNCTION();
		ImGui::PushFont(ImGui::GetIO().FontDefault);

		if (ImGui::BeginPopupContextItem("MeshSelect")) { // Select mesh
			if (ImGui::Button("Select other mesh")) {
				std::string filePath;
				auto resRaw = WdeFileUtils::readFileDialog("json", filePath);

				if (!filePath.empty() && !resRaw.empty()) {
					// Remove old material
					if (_mesh != nullptr)
						WaterDropEngine::get().getResourceManager().release(_mesh->getPath());

					// Add new material
					auto& wde = WaterDropEngine::get();
					_meshName = json::parse(resRaw)["name"];
					_mesh = wde.getResourceManager().load<resource::Mesh>(wde.getInstance().getScene()->getPath() + "data/meshes/" + _meshName + ".json");
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextItem("MaterialSelect")) { // Select mesh
			if (ImGui::Button("Select other material")) {
				std::string filePath;
				auto resRaw = WdeFileUtils::readFileDialog("json", filePath);

				if (!filePath.empty() && !resRaw.empty()) {
					// Remove old material
					if (_material != nullptr)
						WaterDropEngine::get().getResourceManager().release(_material->getPath());

					// Add new material
					auto& wde = WaterDropEngine::get();
					_materialName = json::parse(resRaw)["name"];
					_material = wde.getResourceManager().load<resource::Material>(wde.getInstance().getScene()->getPath() + "data/materials/" + _materialName + ".json");
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::Separator();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		// Mesh
		if (_mesh == nullptr)
			ImGui::Text(" No mesh selected.");
		else
			ImGui::Text(" Mesh name : \"%s\".", _mesh->getName().c_str());
		ImGui::OpenPopupOnItemClick("MeshSelect", ImGuiPopupFlags_MouseButtonRight);


		// Material
		if (_material == nullptr)
			ImGui::Text(" No material selected.");
		else
			ImGui::Text(" Material name : \"%s\".", _material->getName().c_str());
		ImGui::OpenPopupOnItemClick("MaterialSelect", ImGuiPopupFlags_MouseButtonRight);
		ImGui::PopFont();
	}

	json MeshRendererModule::serialize() {
		WDE_PROFILE_FUNCTION();
		json jData;
		jData["material"] = _materialName;
		jData["mesh"] = _meshName;
		return jData;
	}
}
