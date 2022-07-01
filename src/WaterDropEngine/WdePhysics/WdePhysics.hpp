#pragma once

#include "../WdeCore/Core/Module.hpp"

namespace wde::physics {
	class WdePhysics : public core::Module {
		public:
			// Module commands
			explicit WdePhysics(std::shared_ptr<core::Subject> moduleSubject);
			void start();
			void tick() override;
			void cleanUp() override;

			// Notifications
			void onNotify(const core::Event &event) override;
	};
}
