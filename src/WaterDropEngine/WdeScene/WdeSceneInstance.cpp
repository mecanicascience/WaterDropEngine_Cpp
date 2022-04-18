#include "WdeSceneInstance.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::scene {
	void WdeSceneInstance::tick() {
		WDE_PROFILE_FUNCTION();

		// Remove chunks that need to be removed
		while (!_removingChunks.empty()) {
			auto id = _removingChunks.front();
			if (_activeChunks.contains(id))
				_activeChunks.erase(id);
			_removingChunks.pop();
		}

		// Tick for chunks
		for (auto& c : _activeChunks)
			c.second->tick();
	}

	void WdeSceneInstance::cleanUp() {
		WDE_PROFILE_FUNCTION();

		// Clear chunks list
		_activeChunks.clear();
		while (!_removingChunks.empty())
			_removingChunks.pop();

		// Remove references
		_selectedGameObjectChunkID = {0, 0};
		_selectedGameObject = nullptr;
		_activeCamera = nullptr;
	}


	void WdeSceneInstance::onNotify(const core::Event& event) {
#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::createGUI()");

			// Create a game objects list tab
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiID dockIDRight = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.19f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Scene Components", dockIDRight);

			// Create a properties list
			ImGuiID dockIDProperties = ImGui::DockBuilderSplitNode(dockIDRight, ImGuiDir_Down, 0.75f, nullptr, &dockIDRight);
			ImGui::DockBuilderDockWindow("Properties", dockIDProperties);
		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			WDE_PROFILE_SCOPE("wde::scene::WdeSceneInstance::onNotify::drawGUI()");

			// Draw GUI for selected chunk
			// TODO Change selected chunk
			getChunk(_selectedGameObjectChunkID)->drawGUI();
		}
#endif
	}

	Chunk* WdeSceneInstance::getChunk(glm::ivec2 chunkID) {
		// Chunk found
		if (_activeChunks.contains(chunkID))
			return _activeChunks.at(chunkID).get();

		// Not found
		auto ch = std::make_shared<Chunk>(this, chunkID);
		_activeChunks.emplace(chunkID, ch);
		return ch.get();
	}

	void WdeSceneInstance::removeChunk(glm::ivec2 chunkID) {
		_removingChunks.push(chunkID);
	}
}
