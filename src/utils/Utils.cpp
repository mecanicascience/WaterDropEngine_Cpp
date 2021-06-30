#include "Utils.hpp"

namespace wde {
    std::vector<char> Utils::readFile(const std::string &fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);

        // Can't open file
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        // Read file
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        // Close and return
        file.close();
        return buffer;
    }
}
