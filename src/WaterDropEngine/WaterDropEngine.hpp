#pragma once

#include "../wde.hpp"
#include "WdeRender/WdeRender.hpp"
#include "WdeGUI/WdeGUI.hpp"
#include "WdeCore/Core/WaterDropEngineInstance.hpp"

namespace wde {
	/**
	 * Main WDE Game Instance
	 */
	class WaterDropEngine {
		public:
			/** Create a WDE main engine instance */
			static WaterDropEngine& get() {
				static auto *instance = new WaterDropEngine();
				return *instance;
			}


			void start(WaterDropEngineInstance& instance) {
				// Create logging service
				logger::LoggerHandler::initialize("logs/");
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Initializing program ========" << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Initializing program." << logger::endl;

				// Setup
				WDE_PROFILE_BEGIN_SESSION("Initialization", "profiler/profiler_init.json");
				// ===== CREATE MODULE SUBJECT =====
				_subject = std::make_shared<core::Subject>("core Subject");


				// ===== INIT MODULES IN ORDER =====
				// Renderer core
				_render = std::make_shared<render::WdeRender>(_subject);
				_render->start();
				_subject->addObserver(_render);

				// GUI core
				_gui = std::make_shared<gui::WdeGUI>(_subject);
				_subject->addObserver(_gui);

				// Init engine instance
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Initializing engine instance." << logger::endl;
				instance.initialize();

				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of initialization ========" << logger::endl << logger::endl;
				WDE_PROFILE_END_SESSION();

				// Start
				WDE_PROFILE_BEGIN_SESSION("Running", "profiler/profiler_run.json");
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Starting engine ========" << logger::endl;

				// Run
				while (_render->shouldRun()) {
					logger::log() << "====== Updating new frame. ======" << logger::endl;

					// Poll glfw events
					logger::log() << "Polling GLFW events." << logger::endl;
					glfwPollEvents();

					// Update for engine instance
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Updating for engine instance." << logger::endl;
					instance.update();

					// Tick for modules
					logger::log() << "Ticking for modules." << logger::endl;
					_render->tick();
					_gui->tick();

					// Render for engine instance
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Rendering for engine instance." << logger::endl;
					instance.renderWDE();

					logger::log() << "====== End of tick. ======\n\n" << logger::endl;
				}

				// Wait until every used device are ready
				_render->getInstance().waitForDevicesReady();
				WDE_PROFILE_END_SESSION();


				// Clean up WaterDropEngine
				WDE_PROFILE_BEGIN_SESSION("Cleaning Up", "profiler/profiler_cleanup.json");
				// ===== DELETE MODULES IN REVERSE ORDER =====
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up modules ========" << logger::endl;
				_gui->cleanUp();
				_render->cleanUp();

				// ==== DELETE MODULE COMMUNICATION SERVICE ==
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up ended ========" << logger::endl;
				_subject.reset();

				// Init engine instance
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Cleaning up engine instance." << logger::endl;
				instance.cleanUp();

				// Clean up core and logger
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Closing program." << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of program ========" << logger::endl << logger::endl;
				WDE_PROFILE_END_SESSION();
				logger::LoggerHandler::cleanUp();
			}


			// Modules getters
			render::WdeRender& getRender() { return *_render; }
			gui::WdeGUI& getGUI() { return *_gui; }


		private:
			// Modules
			std::shared_ptr<render::WdeRender> _render;
			std::shared_ptr<gui::WdeGUI> _gui;

			// Modules communication subject
			std::shared_ptr<core::Subject> _subject;

			// Constructor
			explicit WaterDropEngine() = default;
	};
}
