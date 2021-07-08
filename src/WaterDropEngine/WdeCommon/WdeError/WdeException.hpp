#pragma once

#include <exception>
#include <string>
#include <utility>

namespace wde {
	class WdeException : public std::exception {
		public:
			WdeException(std::string message) : _message{std::move(message)} {}

			/**
			 * Describes the error with a verbose message
			 * @return A formatted string
			 */
			virtual const char* what() const noexcept override {
				return _message.c_str();
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
			std::string _message;
	};
}
