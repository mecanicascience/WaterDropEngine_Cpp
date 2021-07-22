#pragma once

#include <exception>
#include <string>
#include <utility>

#include "../../WdeCommon/WdeError/WdeStatus.hpp"
#include "../WdeLogger/Logger.hpp"

namespace wde {
	class WdeException : public std::exception {
		public:
			WdeException(std::string message, LoggerChannel channel) : _message{std::move(message)}, _channel{channel} {};

			/**
			 * Describes the error with a verbose message
			 * @return A formatted string
			 */
			const char* what() const noexcept override {
				return _message.c_str();
			}

			/**
			 * @return The corresponding error channel
			 */
			const LoggerChannel getChannel() const noexcept {
				return _channel;
			}

			/**
			 * Converts a status to a int status value
			 * @param status
			 * @return An integer (usually 0 for a sucess, 1 for an error)
			 */
			static int toInt(WdeStatus status) {
				switch (status) {
					case WdeStatus::WDE_ERROR:
						return 1;  // Error code = 1
					case WdeStatus::WDE_SUCCESS:
						return 0;  // Success code = 0
				}

				return 1; // Return error if no type found
			}


		private:
			/** The error message */
			std::string _message;
			/** The corresponding channel */
			LoggerChannel _channel;
	};
}
