#include "WdeFileUtils.hpp"

namespace wde {
	std::vector<char> WdeFileUtils::readFile(const std::string &fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		// Can't open file
		if (!file.is_open())
			throw WdeException("Failed to open file '" + fileName + "'.", LoggerChannel::COMMON);

		// Read file
		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), (std::streamsize) fileSize);

		// Close and return
		file.close();
		return buffer;
	}
}
