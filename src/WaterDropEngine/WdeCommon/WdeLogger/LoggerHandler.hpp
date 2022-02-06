#pragma once

#include <iostream>
#include <unordered_map>
#include <utility>
#include <windows.h>
#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>


namespace wde {
	/** Log level of the messages */
	enum class LogLevel {
		/** Log only errors */                ERR,
		/** Log errors and warnings */        WARN,
		/** Log errors, warnings and infos */ INFO,
		/** Log everything */                 DEBUG
	};

	/** List of every logger channel */
	enum class LogChannel {
		/** Engine main structure */  CORE,
		/** Debug channel messages */ DEBUG,
		/** The rendering engine */   RENDER,
		/** The GUI drawing engine */ GUI
	};


	/**
	 * Represents an instance of a logger
	 */
	namespace logger {
		// Name of the corresponding log levels and channels in same order as in enum
		extern std::string logLevelNames[];
		extern std::string logChannelNames[];


		/** A character to end the line of the current log */
		static const char endl = '\n';

		class LoggerHandler {
			public:
				/** Initialize the LoggerHandler - Must be called only one time by one thread. */
				static void initialize(const std::string& filepath, int logLevel);

				/** Clean-up the LoggerHandler - Must be called only one time by one thread. */
				static void cleanUp();


				/**
				 * Log the given string to the screen and store it
				 * @tparam T
				 * @param message
				 */
				template <typename T>
				void logMsg(T& message) {
					// Check if log level enabled
					if (_level > _logLevel) {
						_log_mutex.lock();
						message.str();
						message.flush();
						_log_mutex.unlock();
						return;
					}

					// Lock writing
					_log_mutex.lock();

					// Format message
					std::string msg =
							" [WDE]\t[" + logLevelNames[_level] + "]"
							+ "\t[" + logChannelNames[_channel] + "-ENGINE]\t"
							+ message.str();
					std::cout << msg;
					message.flush();

					// Unlock writing
					_log_mutex.unlock();

					// Write to file
					if (_logFileInitialized) {
						_logFile << msg;
						_logFile.flush();
					}

					// Throw error
					if (_level == static_cast<int>(LogLevel::ERR))
						throw std::runtime_error("Fatal error. See logs above.");
				}


				/**
				 * Create a new LoggerHandler instance
				 * @param level Message info level
				 * @param channel Origin module name
				 * @return The logger instance
				 */
				LoggerHandler& operator()(const LogLevel& level, const LogChannel& channel) {
					_level = static_cast<int>(level);
					_channel = static_cast<int>(channel);
					return *this;
				}

				/**
				 * Create a new default LoggerHandler instance (level = debug, channel = main)
				 * @return The logger instance
				 */
				LoggerHandler& operator()() {
					_level = static_cast<int>(LogLevel::DEBUG);
					_channel = static_cast<int>(LogChannel::DEBUG);
					return *this;
				}


			private:
				// LoggerHandler description infos
				int _level;
				int _channel;

				// Mutex to allow thread-safe writings
				static std::mutex _log_mutex;

				// Store logs in folder
				static int _logLevel;
				static std::ofstream _logFile;
				static bool _logFileInitialized;
		};

		// Logger global instance
		extern LoggerHandler log;
	}
}
