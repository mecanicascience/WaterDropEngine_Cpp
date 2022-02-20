#pragma once

namespace wde::core {
	/**
	 * Data of a notification given in a subject
	 */
	class Event {
		public:
			/** Channel provider of the event */
			LogChannel channel {};

			/** Name of the event */
			std::string name {};

			/** Data payload of the event */
			void *data {};
	};
}

