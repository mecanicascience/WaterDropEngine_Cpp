#pragma once

#include <iostream>
#include <unordered_map>
#include <utility>
#include <windows.h>
#include <vector>
#include <fstream>
#include <chrono>
#include "../WdeUtils/Constants.hpp"
#include "../WdeError/WdeStatus.hpp"

namespace wde {
	/** List of different LoggerChannels */
	enum class LoggerChannel {
		MAIN,
		RENDERING_ENGINE,
		COMMON,
		INPUT_MANAGER,
		RENDERING_STRUCTURES,
		GUI
	};


	class Logger {
		public:
	        // Base parameters
			/** Verbose log level class */
			enum class LoggerLogLevel {
				/** Log only errors */                ERR,
				/** Log errors and warnings */        WARN,
				/** Log errors, warnings and infos */ INFO,
				/** Log everything */                 DEBUG
			};

			/**
			 * Initialize a new Logger with a new config
			 * @param filepath
			 * @param logLevel The LoggerLogLevel log output level
			 * @param activatedChannels The list of every activated logged channels
			 */
			static void initialize(const std::string& filepath, LoggerLogLevel& logLevel, std::vector<LoggerChannel>& activatedChannels) {
				Logger::_logLevel = logLevel;
				Logger::_activatedChannels = activatedChannels;

				// Gets time as format %Y.%m.%d-%H.%M.%S
                std::time_t rawtime;
                std::tm* timeinfo;
                char buffer [80];
                std::time(&rawtime);
                timeinfo = std::localtime(&rawtime);
                std::strftime(buffer,80,"%Y.%m.%d-%H.%M.%S", timeinfo);

				// Initialize log file
                _logFile.open(filepath + "logs_" + buffer + ".txt");
                if (!_logFile.is_open())
                    throw std::runtime_error("Couldn't open log file.");

                // Log header
                char buffer2 [80];
                std::strftime(buffer2, 80, "%d/%m/%Y-%H:%M:%S", timeinfo);
                _logFile << "  ======================================================================\n"
                      << "    Begin Output log (" << buffer2 << ") : "
                      << Constants::APPLICATION_NAME << " - " << Constants::APPLICATION_VERSION_FORMATTED
                      << "\n  ======================================================================\n\n";
                _logFile.flush();

                _logFileInitialized = true;
			}

			/**
			 * Clean up the Logger and close the log file
			 */
			static void cleanUp() {
                _logFileInitialized = false;

                std::time_t rawtime;
                std::tm* timeinfo;
                char buffer [80];
                std::time(&rawtime);
                timeinfo = std::localtime(&rawtime);
                std::strftime(buffer, 80, "%d/%m/%Y-%H:%M:%S", timeinfo);

                // Log footer
                _logFile << "  ======================================================================\n"
                         << "     End Output log  (" << buffer << ") : "
                         << Constants::APPLICATION_NAME << " - " << Constants::APPLICATION_VERSION_FORMATTED
                         << "\n  ======================================================================\n\n";
                _logFile.flush();

                // Close file
                _logFile.close();
			}


			// Output functions
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
	        // Core parameters
			/** List of activated channels */
			static LoggerLogLevel _logLevel;
			static std::vector<LoggerChannel> _activatedChannels;
            static std::ofstream _logFile;
            static bool _logFileInitialized;


			// Helper functions
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


            // Log output functions
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

