#pragma once

#include "../WdeCore/WdeModule.hpp"

namespace wde::gui {
	class WdeGUI : public WdeModule::Module<WdeGUI> {
		// Register module
		inline static const int MODULE_REGISTERED = RegisterModule("gui", Stage::NORMAL);

		public:
			/** Initialize the module */
			void initialize() override;
			/** On engine ticks */
			void tick() override;
			/** Clean up the module */
			void cleanUp() override;
	};
}
