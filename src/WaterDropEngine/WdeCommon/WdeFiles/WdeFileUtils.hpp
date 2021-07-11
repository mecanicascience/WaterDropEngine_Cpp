#pragma once

#include <vector>
#include <ios>
#include <fstream>

#include "../WdeError/WdeException.hpp"

namespace wde {
	class WdeFileUtils {
		public:
			/**
			 * @param fileName The path of the file from the root of the project
			 * @return The content of the provided file
			 */
			static std::vector<char> readFile(const std::string &fileName);
	};
}
