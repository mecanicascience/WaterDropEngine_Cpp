#pragma once

#include "../../../wde.hpp"
#include "../../WdeRender/WdeRender.hpp"
#include "../../WdeGUI/WdeGUI.hpp"

namespace wde::core {
	class Core {
		public:
			explicit Core() {
				// ===== CREATE MODULE SUBJECT =====
				_subject = std::make_shared<core::Subject>("Core Subject");


				// ===== INIT MODULES IN ORDER =====
				// Renderer Core
				_render = std::make_shared<WdeRender>(_subject);
				_subject->addObserver(_render);

				// GUI Core
				_gui = std::make_shared<WdeGUI>(_subject);
				_subject->addObserver(_gui);
			}

			void start() {
				// INITIALIZE
				// Payload message test
				_subject->notify({"test"});
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== End of initialization ========" << logger::endl << logger::endl;


				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Starting engine ========" << logger::endl;
				// RUN HERE
				// ...
				logger::log(LogLevel::INFO, LogChannel::CORE) << "======== Program ended ========" << logger::endl << logger::endl;
			}

			~Core() {
				// ===== DELETE MODULES IN REVERSE ORDER =====
				_gui->cleanUp();
				_render->cleanUp();

				// ==== DELETE MODULE COMMUNICATION SERVICE ==
				_subject.reset();
			}



		private:
			// Modules
			std::shared_ptr<WdeRender> _render;
			std::shared_ptr<WdeGUI> _gui;

			// Modules communication subject
			std::shared_ptr<core::Subject> _subject;
	};
}
