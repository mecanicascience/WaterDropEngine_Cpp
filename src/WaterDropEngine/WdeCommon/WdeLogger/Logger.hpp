#pragma once

#include <iostream>
#include <unordered_map>
#include <utility>
#include <windows.h>
#include <vector>

namespace wde {
	/** List of different LoggerChannels */
	enum class LoggerChannel {
		MAIN,
		RENDERING_ENGINE,
		COMMON
	};


	class Logger {
		public:
			/** Verbose log level class */
			enum class LoggerLogLevel {
				/** Log only errors */                ERR,
				/** Log errors and warnings */        WARN,
				/** Log errors, warnings and infos */ INFO,
				/** Log everything */                 DEBUG
			};

			/**
			 * Initialize a new Logger with a new config
			 * @param logLevel The LoggerLogLevel log output level
			 * @param activatedChannels The list of every activated logged channels
			 */
			static void initialize(LoggerLogLevel& logLevel, std::vector<LoggerChannel>& activatedChannels) {
				Logger::_logLevel = logLevel;
				Logger::_activatedChannels = activatedChannels;
			}


			/**
			 * Prints a debug message to the console
			 * @param message The std::string message
			 * @param channel The channel of the message
			 */
			static void debug(const std::string &message, LoggerChannel channel);

			/**
			 * Prints an informative message to the console
			 * @param message The std::string message
			 * @param channel The channel of the message
			 */
			static void info(const std::string &message, LoggerChannel channel);

			/**
			 * Prints a warning message to the console
			 * @param message The std::string message
			 * @param channel The channel of the message
			 */
			static void warn(const std::string &message, LoggerChannel channel);

			/**
			 * Prints an error message to the console
			 * @param message The std::string message
			 * @param channel The channel of the message
			 */
			static void err(const std::string &message, LoggerChannel channel);



		private:
			/** List of activated channels */
			static LoggerLogLevel _logLevel;
			static std::vector<LoggerChannel> _activatedChannels;

			/**
			 * @param channel The channel to check
			 * @param providedLogLevel The log level of the message to be shown
			 * @return true If the provided inputs are valid and can be show on screen
			 */
			static bool checkValidInput(LoggerChannel channel, LoggerLogLevel providedLogLevel);

			/**
			 * Creates the outputted debug message from the specified infos
			 * @param message Message string
			 * @param type Type of the error
			 * @param channel Label of the channel
			 * @return The formatted string
			 */
			static std::string getFormatedMessage(const std::string &message, const std::string &type, LoggerChannel channel);

			/**
			 * @param channel
			 * @return The name of the channel
			 */
			static std::string getNameOf(LoggerChannel channel);



			/**
			 * Outputs a message to the console
			 * @param message The message
			 */
			static void outputMessage(const std::string &message);

			/**
			 * Outputs an error to the console
			 * @param message
			 */
			static void outputError(const std::string &message);
		};
}
