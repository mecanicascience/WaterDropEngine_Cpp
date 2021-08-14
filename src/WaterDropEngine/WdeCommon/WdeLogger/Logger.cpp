#include "Logger.hpp"
#include "../../WdeGUI/GUITheme.hpp"

namespace wde {
	// Main thread
	std::thread::id Logger::MAIN_THREAD_ID = std::this_thread::get_id();

    // Log file not initialized by default
    bool Logger::_logFileInitialized = false;
    std::ofstream Logger::_logFile;

	// Initialize Log Level values
	Logger::LoggerLogLevel Logger::_logLevel = Logger::LoggerLogLevel::ERR;
	std::vector<LoggerChannel> Logger::_activatedChannels = { LoggerChannel::MAIN };

	// Initialize log messages list
	std::vector<LogMessage> Logger::_logLines {};

	std::unordered_map<LoggerChannel, ImColor> Logger::_channelsColors = {
		{LoggerChannel::MAIN, gui::GUITheme::colorWhiteMinor},
		{LoggerChannel::RENDERING_ENGINE, gui::GUITheme::colorBlueMinor},
		{LoggerChannel::COMMON, gui::GUITheme::colorOrangeMinor},
		{LoggerChannel::INPUT_MANAGER, gui::GUITheme::colorPurpleMinor},
		{LoggerChannel::GUI, gui::GUITheme::colorRedMinor},
		{LoggerChannel::SCENE, gui::GUITheme::colorGreenMinor}
	};



	// Main functions
	void Logger::debug(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::DEBUG))
			return;

		// Outputs message
		outputMessage(getFormatedMessage(message, " DEBUG ", channel));

		// Add line to the log line list
		_logLines.emplace_back(message, "debug", channel);

		// If more line than threshold, clear first lines
		if (std::this_thread::get_id() == MAIN_THREAD_ID && _logLines.size() > Constants::GUI_CONSOLE_MAX_LINES)
			_logLines.erase(_logLines.begin());
	}

	void Logger::info(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::INFO))
			return;

		outputMessage(getFormatedMessage(message, " INFO  ", channel));

		// Add line to the log line list
		_logLines.emplace_back(message, "info", channel);

		// If more line than threshold, clear first lines
		if (std::this_thread::get_id() == MAIN_THREAD_ID && _logLines.size() > Constants::GUI_CONSOLE_MAX_LINES)
			_logLines.erase(_logLines.begin());
	}

	void Logger::warn(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::WARN))
			return;

		outputMessage(getFormatedMessage(message, "WARNING", channel));

		// Add line to the log line list
		_logLines.emplace_back(message, "warning", channel);

		// If more line than threshold, clear first lines
		if (std::this_thread::get_id() == MAIN_THREAD_ID && _logLines.size() > Constants::GUI_CONSOLE_MAX_LINES)
			_logLines.erase(_logLines.begin());
	}

	void Logger::err(const std::string &message, LoggerChannel channel) {
		// Always print errors
		outputError(getFormatedMessage(message, " ERROR ", channel));

		// Add line to the log line list
		_logLines.emplace_back(message, "error", channel);

		// If more line than threshold, clear first lines
		if (std::this_thread::get_id() == MAIN_THREAD_ID && _logLines.size() > Constants::GUI_CONSOLE_MAX_LINES)
			_logLines.erase(_logLines.begin());

		// Throw error
		throw std::runtime_error("See above logs.");
	}



	bool Logger::checkValidInput(LoggerChannel channel, LoggerLogLevel providedLogLevel) {
		return
				!_activatedChannels.empty() // Channels not empty
				&& std::find(_activatedChannels.begin(), _activatedChannels.end(), channel) != _activatedChannels.end() // Channels contains channel
				&& ( // ProvidedLogLevel > _logLevel
						(providedLogLevel == LoggerLogLevel::DEBUG && (_logLevel == LoggerLogLevel::DEBUG))
						|| (providedLogLevel == LoggerLogLevel::INFO  && (_logLevel == LoggerLogLevel::DEBUG || _logLevel == LoggerLogLevel::INFO))
						|| (providedLogLevel == LoggerLogLevel::WARN  && (_logLevel == LoggerLogLevel::DEBUG || _logLevel == LoggerLogLevel::INFO || _logLevel == LoggerLogLevel::WARN))
						||  providedLogLevel == LoggerLogLevel::ERR  // Always displays errors
				);
	}




	// Message formatting
	std::string Logger::getFormatedMessage(const std::string &message, const std::string &type, LoggerChannel channel) {
		return " [WDE] [" + type + "] [" + getNameOf(channel) + "-CHANNEL] " + message;
	}

	std::string Logger::getNameOf(LoggerChannel channel) {
		switch (channel) {
			case LoggerChannel::COMMON:
				return "COMMON";
			case LoggerChannel::RENDERING_ENGINE:
				return "RENDERING-ENGINE";
			case LoggerChannel::MAIN:
				return "MAIN";
			case LoggerChannel::INPUT_MANAGER:
				return "INPUT-MANAGER";
			case LoggerChannel::GUI:
				return "GUI";
			case LoggerChannel::SCENE:
				return "SCENE";
		}

		// Channel not found
		outputMessage(getFormatedMessage("A channel is not specified and using the Logger class.", "WARNING", LoggerChannel::MAIN));
		return "CHANNEL-NOT-SPECIFIED";
	}



	// Message output
	void Logger::outputMessage(const std::string &message) {
	    // Outputs the message
		std::cout << message << std::endl;

		// Stores the message to the log file
		if (_logFileInitialized) {
            _logFile << message << std::endl;
            _logFile.flush();
		}
	}

	void Logger::outputError(const std::string &message) {
        // Outputs the error
		std::cerr << message << std::endl;

        // Stores the error to the log file
        if (_logFileInitialized) {
            _logFile << message << std::endl;
            _logFile.flush();
        }
	}
}
