#pragma once

#include "../wde.hpp"
#include "WdeCore/WdeInstance.hpp"

namespace wde {
	class WaterDropEngine : NonCopyable {
		public:
			/** Creates the engine */
            explicit WaterDropEngine();
			/** Initialize the program */
			WdeStatus initialize();
			/** Runs the program */
			WdeStatus run();

		private:
			/** The WaterDropInstance */
			std::unique_ptr<WdeInstance> _instance;
	};
}
