#pragma once

#include "../WdeLogger/LoggerHandler.hpp"

namespace wde {
	/**
	 * Handles a WaterDropEngine exception format.
	 * Use format : throw WdeException(LogChannel::CHANNEL, "ERROR DESCRIPTION");
	 */
	class WdeException : public std::exception {
		public:
			explicit WdeException(LogChannel channel, const std::string& message)
				: _message{" [WDE] [" + logger::logChannelNames[static_cast<int>(channel)] + "] " + message} {};

			/**
			 * Describes the error with a verbose message
			 * @return A formatted string
			 */
			const char* what() const noexcept override {
				return _message.c_str();
			}


		private:
			/** The error message */
			std::string _message;
	};
}
