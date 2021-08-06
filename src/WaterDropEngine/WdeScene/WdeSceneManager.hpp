#pragma once

#include "../WdeCore/WdeModule.hpp"
#include "../../wde.hpp"
#include "scenes/Scene.hpp"

#include <memory>
#include <utility>

namespace wde::scene {
	class WdeSceneManager : public WdeModule::Module<WdeSceneManager> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("sceneManager", Stage::POST);

		public:
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


			// Getters and setters
			/**
			 * Sets the current scene to a new scene
			 * @param scene The new scene
			 */
			void setScene(std::unique_ptr<Scene> scene);
			std::unique_ptr<Scene>& getActiveScene() { return _scene; }


		private:
			std::unique_ptr<Scene> _scene {};
			bool _hasStarted = false;
	};
}
