#pragma once

#include "../../../wde.hpp"
#include "Event.hpp"

namespace wde::core {
	class Observer {
		public:
			/**
			 * Called when a new event occurs in the registered subject
			 * @param event
			 */
			virtual void onNotify(Event event) = 0;
	};
}
