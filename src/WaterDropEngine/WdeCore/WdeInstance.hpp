#pragma once

#include "../../wde.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde {
	/**
	 * A unique WaterDropEngine instance. This class will initialize every core engines.
	 */
	class WdeInstance : NonCopyable {
		public:
			/**
			 * Initialize the WdeInstance and all of its important different class components
			 */
			WdeStatus initialize();

			/** Run the WdeInstance and all of its important different class components */
			WdeStatus run();

			/** CleanUp the WdeInstance and all of its important different class components */
			WdeStatus cleanUp();


	private:
			/** The main renderer engine */
			renderEngine::WdeRenderEngine wdeRenderingEngine {};
	};
}
