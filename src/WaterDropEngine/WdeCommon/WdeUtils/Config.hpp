#pragma once

// This macro allow the program to record profiling
#define WDE_USE_PROFILING

namespace wde {
	class Config {
		public:
			/** LOG OUTPUT LEVEL (0 = error - 1 = + warnings - 2 = + info - 3 = + debug) */
			static const int LOG_LEVEL = 3;
	};
}

