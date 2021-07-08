#pragma once

#include "../WdeCommon/WdeError/WdeStatus.hpp"

namespace wde::renderEngine {
	class WdeRenderEngine {
		public:
			// Constructors
			WdeRenderEngine() = default;
			~WdeRenderEngine() = default;

			/** Creates a new rendering engine instance */
			WdeStatus initialize();
			/** Called when a new tick on the gameLoop happend */
			WdeStatus tick();
			/** Cleans up the rendering engine instance */
			WdeStatus cleanUp();

			/** @return true if the application must stop */
			bool shouldEnd();
	};
}
