#pragma once

#include "Module.hpp"

namespace wde::scene {
	class EmptyModule : public Module {
		public:
			explicit EmptyModule() : Module("Empty") {}

		// An empty module that does nothing
	};
}

