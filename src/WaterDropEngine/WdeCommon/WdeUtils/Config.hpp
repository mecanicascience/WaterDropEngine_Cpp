#pragma once

// Set engine mode (1 = production, 2 = debug)
#ifndef WDE_ENGINE_MODE
#define WDE_ENGINE_MODE 2
#endif

namespace wde {
	class Config {
		public:
			/** LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) */
			static const int LOG_LEVEL = 3;
	};
}

