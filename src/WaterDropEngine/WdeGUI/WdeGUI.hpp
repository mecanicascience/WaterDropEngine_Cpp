#pragma once

#include "../WdeCore/Core/Module.hpp"

namespace wde {
	class WdeGUI : public core::Module {
		public:
			// Module commands
			explicit WdeGUI(std::shared_ptr<core::Subject> moduleSubject);
			void tick() override;
			void cleanUp() override;

			// Notifications
			void onNotify(core::Event event) override;
	};
}
