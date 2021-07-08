#include "Logger.hpp"

namespace wde {
	// Initialize Log Level values
	Logger::LoggerLogLevel Logger::_logLevel = Logger::LoggerLogLevel::DEBUG;
	std::vector<LoggerChannel> Logger::_activatedChannels = { LoggerChannel::MAIN };



	// Main functions
	void Logger::debug(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::DEBUG))
			return;
		outputMessage(getFormatedMessage(message, " DEBUG ", channel));
	}

	void Logger::info(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::INFO))
			return;
		outputMessage(getFormatedMessage(message, " INFO  ", channel));
	}

	void Logger::warn(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::WARN))
			return;
		outputMessage(getFormatedMessage(message, "WARNING", channel));
	}

	void Logger::err(const std::string &message, LoggerChannel channel) {
		if (!checkValidInput(channel, LoggerLogLevel::ERR))
			return;
		outputError(getFormatedMessage(message, " ERROR ", channel));
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
		}

		// Channel not found
		outputMessage(getFormatedMessage("A channel is not specified and using the Logger class.", "WARNING", LoggerChannel::MAIN));
		return "CHANNEL-NOT-SPECIFIED";
	}



	// Message output
	void Logger::outputMessage(const std::string &message) {
		std::cout << message << std::endl;
	}

	void Logger::outputError(const std::string &message) {
		std::cerr << message << std::endl;
	}
}
