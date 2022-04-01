#pragma once

#include <thread>

#include "../wde.hpp"
#include "WdeRender/WdeRender.hpp"
#include "WdeGUI/WdeGUI.hpp"
#include "WdeCore/Core/WdeInstance.hpp"
#include "WdeCommon/WdeUtils/FPSUtils.hpp"
#include "WdeScene/WdeScene.hpp"
#include "WdeInput/InputManager.hpp"
#include "WdeRender/descriptors/DescriptorBuilder.hpp"
#include "WdeScene/gizmo/GizmoManager.hpp"
#include "WdeResourceManager/WdeResourceManager.hpp"

namespace wde {
	/**
	 * Main WDE Game Instance
	 */
	class WaterDropEngine : public NonCopyable {
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
				WDE_PROFILE_BEGIN_SESSION("Initialization", "logs/profiler_init.json");
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

				// Input manager core
				_input = std::make_shared<input::InputManager>();

				// Scene core
				_scene = std::make_shared<scene::WdeScene>(_subject);
				_subject->addObserver(_scene);

				// Resource Manager
				_resourceManager = std::make_shared<resource::WdeResourceManager>(_subject);
				_subject->addObserver(_resourceManager);

				// Init engine instance
				#ifdef WDE_ENGINE_MODE_DEBUG
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Initializing engine instance." << logger::endl;
				#else
					std::cout << "Initializing engine instance." << std::endl;
				#endif

				// Initialize instance on a different thread
				std::thread t ([](WdeInstance* instanceRef) {instanceRef->initialize();}, &instance);
				t.join();

				// Load scene
				_scene->loadScene();
				_gui->addObserver(instance.getScenePointer(), true);
				_gui->addObserver(_resourceManager);
				_gui->addObserver(_scene);

				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of initialization ========" << logger::endl << logger::endl;
				WDE_PROFILE_END_SESSION();

				// Start
				WDE_PROFILE_BEGIN_SESSION("Running", "logs/profiler_run.json");
				#ifdef WDE_ENGINE_MODE_DEBUG
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Starting engine." << logger::endl;
				#else
					std::cout << "Starting engine." << std::endl;
				#endif

				// Run
				FPSUtils fpsManager {};
				while (_render->shouldRun()) {
					{
						WDE_PROFILE_SCOPE("wde::WaterDropEngine::tick()::glfwPollEvents");
						logger::log(LogLevel::INFO, LogChannel::CORE) << "====== Updating new frame. ======" << logger::endl;
						fpsManager.update();

						#ifdef WDE_ENGINE_MODE_DEBUG
							// Log FPS
							logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking FPS : " << fpsManager.getFPS() << "." << logger::endl;

							// Display FPS on window title
							if (fpsManager.hasNewValue()) {
								auto windowTitle = Config::APPLICATION_NAME + " - FPS : " + std::to_string(fpsManager.getFPS());
								glfwSetWindowTitle(_render->getWindow().getWindow(), windowTitle.c_str());
							}
						#else
							if (fpsManager.hasNewValue()) {
								std::cout << "Ticking FPS : " << fpsManager.getFPS() << "." << std::endl;
							}
						#endif

						// Poll glfw events
						logger::log(LogLevel::INFO, LogChannel::CORE) << "Polling GLFW events." << logger::endl;
						glfwPollEvents();
					}

					{
						WDE_PROFILE_SCOPE("wde::WaterDropEngine::tick()::tick");
						// Tick
						logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking for modules." << logger::endl;
						_input->tick();
						_render->tick();
						_gui->tick();
						_resourceManager->tick();

						logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking for engine instance." << logger::endl;
						instance.tickInstance();

						logger::log(LogLevel::INFO, LogChannel::CORE) << "Ticking for scene instance." << logger::endl;
						_scene->tick();

						logger::log(LogLevel::INFO, LogChannel::CORE) << "====== End of tick. ======\n\n" << logger::endl;
					}
				}

				// Wait until every used device are ready
				_render->getInstance().waitForDevicesReady();
				WDE_PROFILE_END_SESSION();


				// Clean up WaterDropEngine
				WDE_PROFILE_BEGIN_SESSION("Cleaning Up", "logs/profiler_cleanup.json");
				// ===== DELETE MODULES IN REVERSE ORDER =====
				// Clear engine instance
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up modules ========" << logger::endl;
				#ifdef WDE_ENGINE_MODE_DEBUG
					logger::log(LogLevel::INFO, LogChannel::CORE) << "Cleaning up engine instance." << logger::endl;
				#else
					std::cout << "Cleaning up engine instance." << std::endl;
				#endif
				instance.cleanUpInstance();

				_input.reset();
				_resourceManager->cleanUp();
				_scene->cleanUp();
				_gui->cleanUp();
				scene::GizmoManager::cleanUp();
				_render->cleanUp();

				// ==== DELETE MODULE COMMUNICATION SERVICE ==
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Cleaning up ended ========" << logger::endl;
				_subject.reset();

				// Clean up core and logger
				logger::log(LogLevel::INFO, LogChannel::CORE) << "Closing program." << logger::endl;
				WDE_PROFILE_END_SESSION();
				logger::LoggerHandler::cleanUp();
			}


			// Modules getters
			render::WdeRender& getRender() { return *_render; }
			gui::WdeGUI& getGUI() { return *_gui; }
			WdeInstance& getInstance() { return *_instance; }
			core::Subject& getSubject() { return *_subject; }
			scene::WdeScene& getScene() { return *_scene; }
			input::InputManager& getInput() { return *_input; }
			resource::WdeResourceManager& getResourceManager() { return *_resourceManager; }


		private:
			// Modules
			std::shared_ptr<render::WdeRender> _render;
			std::shared_ptr<gui::WdeGUI> _gui;
			std::shared_ptr<scene::WdeScene> _scene;
			std::shared_ptr<input::InputManager> _input;
			std::shared_ptr<resource::WdeResourceManager> _resourceManager;

			// Modules communication subject
			std::shared_ptr<core::Subject> _subject;

			// Engine instance
			WdeInstance* _instance = nullptr;

			// Constructor
			explicit WaterDropEngine() = default;
	};
}
