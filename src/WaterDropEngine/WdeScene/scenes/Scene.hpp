#pragma once

#include "../../../wde.hpp"

namespace wde::scene {
	class Scene : NonCopyable {
		public:
			// Constructors
			explicit Scene() = default;
			virtual ~Scene() = default;

			// Core functions
			virtual void initialize() = 0;
			virtual void update() = 0;
			virtual void render() = 0;
			virtual void cleanUp() = 0;
	};
}
