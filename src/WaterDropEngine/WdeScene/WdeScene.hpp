#pragma once

#include "../../wde.hpp"
#include "../WdeCore/Core/Module.hpp"

namespace wde::scene {
	/**
	 * Scene handling module
	 */
	class WdeScene : public core::Module {
		public:
			// Module commands
			explicit WdeScene(std::shared_ptr<core::Subject> moduleSubject);
			void tick() override;
			void cleanUp() override;

			// Notifications
			void onNotify(const core::Event& event) override;

			/** Loads a scene at a path */
			static void loadScenePath();
			/** Loads a scene */
			static void loadScene();
	};
}
