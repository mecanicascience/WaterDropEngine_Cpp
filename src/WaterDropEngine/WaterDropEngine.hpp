#pragma once

#include "../wde.hpp"
#include "WdeRender/WdeRender.hpp"
#include "WdeGUI/WdeGUI.hpp"
#include "WdeCore/Core/WdeInstance.hpp"
#include "WdeCommon/WdeUtils/FPSUtils.hpp"

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


			void start(WdeInstance& instance) {
				_instance = &instance;

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
				FPSUtils fpsManager {};
				while (_render->shouldRun()) {
					logger::log(LogLevel::INFO, LogChannel::CORE) << "====== Updating new frame. ======" << logger::endl;
					fpsManager.update();
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking FPS : " << fpsManager.getFPS() << "." << logger::endl;

					// Poll glfw events
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Polling GLFW events." << logger::endl;
					glfwPollEvents();

					// Tick for modules
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking for modules." << logger::endl;
					_render->tick();
					_gui->tick();

					// Render for engine instance
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking for engine instance." << logger::endl;
					instance.tickInstance();

					logger::log() << "====== End of tick. ======\n\n" << logger::endl;
				}

				// Wait until every used device are ready
				_render->getInstance().waitForDevicesReady();
				WDE_PROFILE_END_SESSION();


				// Clean up WaterDropEngine
				WDE_PROFILE_BEGIN_SESSION("Cleaning Up", "profiler/profiler_cleanup.json");
				// ===== DELETE MODULES IN REVERSE ORDER =====
				// Clear engine instance
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up modules ========" << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Cleaning up engine instance." << logger::endl;
				instance.cleanUpInstance();

				_gui->cleanUp();
				_render->cleanUp();

				// ==== DELETE MODULE COMMUNICATION SERVICE ==
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up ended ========" << logger::endl;
				_subject.reset();

				// Clean up core and logger
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Closing program." << logger::endl;
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of program ========" << logger::endl << logger::endl;
				WDE_PROFILE_END_SESSION();
				logger::LoggerHandler::cleanUp();
			}


			// Modules getters
			render::WdeRender& getRender() { return *_render; }
			gui::WdeGUI& getGUI() { return *_gui; }
			WdeInstance& getInstance() { return *_instance; }


		private:
			// Modules
			std::shared_ptr<render::WdeRender> _render;
			std::shared_ptr<gui::WdeGUI> _gui;

			// Modules communication subject
			std::shared_ptr<core::Subject> _subject;

			// Engine instance
			WdeInstance* _instance;

			// Constructor
			explicit WaterDropEngine() = default;
	};
}
