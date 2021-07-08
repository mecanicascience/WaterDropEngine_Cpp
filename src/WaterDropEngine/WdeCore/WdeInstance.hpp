#pragma once

#include "../WdeCommon/WdeError/WdeStatus.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde {
	class WdeInstance {
		public:
			WdeInstance() = default;
			~WdeInstance() = default;

			/** Initialize the WdeInstance and all of its important different class components */
			WdeStatus initialize();

			/** Run the WdeInstance and all of its important different class components */
			WdeStatus run();

			/** CleanUp the WdeInstance and all of its important different class components */
			WdeStatus cleanUp();


		private:
			renderEngine::WdeRenderEngine wdeRenderingEngine {};
	};
}
