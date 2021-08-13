#pragma once

#include <vector>
#include <ios>
#include <fstream>

#include "../WdeError/WdeException.hpp"

namespace wde {
	class WdeFileUtils {
		public:
			// Raw
			/**
			 * @param fileName The path of the file from the root of the project
			 * @return The content of the provided file
			 */
			static std::vector<char> readFile(const std::string &fileName);


			// Dialogs
			/**
			 * Prompt a dialog for the user to select a unique file
			 * @param format The file format allowed
			 * @return The file content
			 */
			static std::vector<char> readFileDialog(const std::string& format);

			/**
			 * Save the file using a prompt dialog
			 * @param fileContent
			 * @param format The file format allowed
			 */
			static void saveFileDialog(const std::string& fileContent, const std::string& format);
	};
}
