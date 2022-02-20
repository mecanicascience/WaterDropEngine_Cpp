#pragma once

#include <sstream>
#include "LoggerHandler.hpp"

namespace wde::logger {
	struct Logger {
		std::stringstream ss;

		Logger() = default;
		Logger(const Logger&) = default;
		Logger& operator=(const Logger&) = delete;
		Logger& operator=(Logger&&) = delete;
		Logger(Logger &&buf) noexcept : ss(move(buf.ss)) {}

		template <typename T>
		Logger& operator<<(T&& message) {
			ss << std::forward<T>(message);
			return *this;
		}

		/** Output the buffer to the logs */
		~Logger() {
			log.logMsg(ss);
		}
	};


	template <typename T>
	Logger operator<<(LoggerHandler& logger, T&& message) {
		Logger buf;
		buf.ss << std::forward<T>(message);
		return buf;
	}
}
