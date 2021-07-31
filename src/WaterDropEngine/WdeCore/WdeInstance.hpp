#pragma once

#include <map>

#include "../../wde.hpp"
#include "WdeModule.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde {
	/**
	 * A unique WaterDropEngine instance. This class will initialize every core modules.
	 */
	class WdeInstance : NonCopyable {
		public:
			/** Initialize the WdeInstance and all of its important different class components */
			WdeStatus initialize();

			/** Run the WdeInstance and all of its important different class components */
			WdeStatus run();

			/** CleanUp the WdeInstance and all of its important different class components */
			WdeStatus cleanUp();


		private:
			/** The list of modules instances ordered by rendering order */
			std::map<int, std::unique_ptr<WdeModule>> _modulesList {};
	};
}
