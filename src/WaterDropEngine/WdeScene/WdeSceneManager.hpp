#pragma once

#include <memory>
#include <utility>
#include <algorithm>
#include <cctype>
#include <string>
#include <thread>

#include "../WdeCore/WdeModule.hpp"
#include "../../wde.hpp"
#include "Scene.hpp"
#include "../WdeCommon/WdeFiles/WdeFileUtils.hpp"

namespace wde::scene {
	class WdeSceneManager : public WdeModule::Module<WdeSceneManager> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("sceneManager", Stage::POST);

		public:
			// Constructors
			~WdeSceneManager() override;

			// Core module functions
			/** Initialize the module */
			void initialize() override;
			/** On engine ticks */
			void tick() override;
			/** Clean up the module */
			void cleanUp() override;


			// GUI functions
			/** Setup the GUI (can be called multiple times - called by the GUI manager) given the ID of the parent gui */
			void setupGUI(ImGuiID &parentID);
			/** Render the GUI elements (called by the GUI manager) */
			void renderGUI();

			// Serializers
			/** Serialize the scene as a json file */
			void serializeScene();
			/** Deserialize the scene given it's json structure */
			void deserializeScene(const json& sceneJSONContent);


			// Getters and setters
			/**
			 * Sets the current scene to a new scene
			 * @param scene The new scene
			 */
			void setScene(std::unique_ptr<Scene> scene);
			std::unique_ptr<Scene>& getActiveScene() { return _scene; }
			/** @return true if the scene is ready to render */
			bool hasScene() {
				if (_scene == nullptr)
					return false;
				return getActiveScene()->isInitialized();
			}


		private:
			std::unique_ptr<Scene> _scene {};

			/** Scene loading and storing threads */
			std::thread _initializeSceneThread;
	};
}
