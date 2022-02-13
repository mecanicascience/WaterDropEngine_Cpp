#include "WdeFileUtils.hpp"

#include "../../lib/portable-file-dialogs/portable-file-dialogs.h"

namespace wde {
	// Raw
	std::vector<char> WdeFileUtils::readFile(const std::string &fileName) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::COMMON) << "Reading file " << fileName << logger::endl;
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		// Can't open file
		if (!file.is_open())
			throw WdeException(LogChannel::COMMON, "Failed to open file '" + fileName + "'.");

		// Read file
		auto fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), (std::streamsize) fileSize);

		// Close and return
		file.close();
		return buffer;
	}


	// Dialogs
	std::vector<char> WdeFileUtils::readFileDialog(const std::string& format) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::COMMON) << "Opening file." << logger::endl;

		auto selection = pfd::open_file("Select a file", ".", { "Format", "*." + format, "All Files", "*" }).result();
		if (!selection.empty())
			return readFile(selection[0]);

		return {};
	}

	void WdeFileUtils::saveFileDialog(const std::string& fileContent, const std::string& format) {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::COMMON) << "Saving file." << logger::endl;

		// Select where to save
		auto destination = pfd::save_file("Save file as", ".", { "Format", "*." + format, "All Files", "*" }, pfd::opt::force_overwrite).result();
		if (!destination.empty()) {
			// Write to file
			std::ofstream out(destination);
			out << fileContent;
			out.close();
		}
	}
}

