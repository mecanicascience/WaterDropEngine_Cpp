#pragma once

#include "WdeCommon/WdeError/WdeStatus.hpp"
#include "WdeCommon/WdeLogger/Logger.hpp"
#include "WdeCommon/WdeError/WdeException.hpp"
#include "WdeCore/WdeInstance.hpp"
#include "WdeRenderingEngine/renderer/renderers/DefaultRenderer.hpp"

namespace wde {
	class WaterDropEngine {
		public:
			WaterDropEngine() = default;
			~WaterDropEngine() = default;

			/** Initialize the program */
			WdeStatus initialize();
	};
}
