#pragma once

#include "WdeCommon/WdeError/WdeStatus.hpp"

namespace wde {
	class WaterDropEngine {
		public:
			WaterDropEngine() = default;
			~WaterDropEngine() = default;

			/** Initialize the program */
			WdeStatus initialize();
	};
}
