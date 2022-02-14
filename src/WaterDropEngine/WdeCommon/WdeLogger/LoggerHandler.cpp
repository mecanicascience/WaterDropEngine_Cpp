#include "LoggerHandler.hpp"
#include "../WdeUtils/Config.hpp"

namespace wde::logger {
	// Log config
	std::string logLevelNames[] = { "ERR", "WARN", "INFO", "DEBUG" };
	std::string logChannelNames[] = { "CORE", "DEBUG", "RENDER", "GUI", "COMMON", "SCENE" };

	// Mutex to allow thread-safe writings
	std::mutex LoggerHandler::_log_mutex;

	// Store logs in folder
	std::ofstream LoggerHandler::_logFile;
	int LoggerHandler::_logLevel;
	bool LoggerHandler::_logFileInitialized;

	// Logger global instance
	LoggerHandler log;


	// Log configuration values
	void LoggerHandler::initialize(const std::string &filepath) {
		// Set local variables
		_logLevel = LOG_LEVEL;

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
		<< " WaterDropEngine "
		<< "\n  ======================================================================\n\n";
		_logFile.flush();

		_logFileInitialized = true;
	}

	void LoggerHandler::cleanUp() {
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
		<< " WaterDropEngine "
		<< "\n  ======================================================================\n\n";
		_logFile.flush();

		// Close file
		_logFile.close();
	}
}
