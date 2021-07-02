#pragma once

#include <fstream>
#include <vector>

namespace wde {
    class Utils {
        public:
            /**
             * @param fileName The path of the file from the root of the project
             * @return The content of the provided file
             */
            static std::vector<char> readFile(const std::string &fileName);
    };
}
