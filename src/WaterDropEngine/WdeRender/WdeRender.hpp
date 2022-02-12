#pragma once

#include "../WdeCore/Core/Module.hpp"
#include "core/CoreWindow.hpp"
#include "core/CoreInstance.hpp"

namespace wde::render {
	class WdeRender : public core::Module {
		public:
			// Module commands
			explicit WdeRender(std::shared_ptr<core::Subject> moduleSubject);
			void start();
			void tick() override;
			void cleanUp() override;

			// Notifications
			void onNotify(core::Event event) override;

			// Getters and setters
			/** @return true if the simulation should continue to run */
			bool shouldRun() const { return _shouldRun; }
			CoreWindow& getWindow() { return *_window; }
			CoreInstance& getInstance() { return *_coreInstance; }


		private:
			/** Reference to the GLFW window */
			std::unique_ptr<CoreWindow> _window;
			/** Reference to the vulkan instance */
			std::unique_ptr<CoreInstance> _coreInstance;

			/** CoreWindow running */
			bool _shouldRun = true;
			/** Tell the renderer to reset elements on next rendering frame */
			bool _windowResized = false;
	};
}
