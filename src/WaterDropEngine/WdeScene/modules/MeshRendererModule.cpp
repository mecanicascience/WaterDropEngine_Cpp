#include "MeshRendererModule.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::scene {
	MeshRendererModule::MeshRendererModule(GameObject &gameObject) : Module(gameObject, "Mesh Renderer", ICON_FA_GHOST) {}

	MeshRendererModule::MeshRendererModule(GameObject &gameObject, const std::string &data) : Module(gameObject, "Mesh Renderer", ICON_FA_GHOST) {
		auto dataJ = json::parse(data);
		auto& wde = WaterDropEngine::get();
		_material = wde.getResourceManager().load<resource::Material>(wde.getInstance().getScene().getPath() + "description/materials/" + dataJ["material"].get<std::string>());
		_mesh = wde.getResourceManager().load<resource::Mesh>(wde.getInstance().getScene().getPath() + "data/meshes/" + dataJ["mesh"].get<std::string>());
	}


	void MeshRendererModule::drawGUI() {
		ImGui::PushFont(ImGui::GetIO().FontDefault);
		// Mesh
		if (_mesh == nullptr)
			ImGui::Text(" No mesh selected.");
		else
			ImGui::Text(" Mesh name : \"%s\".", _mesh->getName().c_str());

		// Material
		if (_material == nullptr)
			ImGui::Text(" No material selected.");
		else
			ImGui::Text(" Material name : \"%s\".", _material->getName().c_str());
		ImGui::PopFont();
	}
}
