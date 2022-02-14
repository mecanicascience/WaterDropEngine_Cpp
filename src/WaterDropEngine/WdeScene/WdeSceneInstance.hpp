#pragma once

#include "../../wde.hpp"

namespace wde::scene {
	/**
	 * Engine scene instance
	 */
	class WdeSceneInstance {
		public:
			// Scene engine methods
			virtual void setup() = 0;
			virtual void update() = 0;
			virtual void cleanUp() = 0;

			// Scene instance methods
			/** Ticking for scene instance (called by WaterDropEngine) */
			void tick();
	};
}
