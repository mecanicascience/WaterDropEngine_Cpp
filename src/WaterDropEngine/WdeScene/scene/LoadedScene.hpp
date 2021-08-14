#pragma once

#include "../Scene.hpp"

namespace wde::scene {
	/**
	 * Class that represents a scene loaded from a file
	 */
	class LoadedScene : public Scene {
		public:
			/**
			 * Create a new loaded scene given a new
			 * @param name
			 */
			LoadedScene(const json& sceneData) : _sceneData(sceneData), Scene(sceneData["name"]) {}

		private:
			/** The loaded json scene data */
			json _sceneData;
	};
}

