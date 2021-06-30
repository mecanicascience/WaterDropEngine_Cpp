#pragma once

#include <vector>
#include <fstream>
#include <iostream>

namespace wde {
    class WdeDefaultGraphicPipeline {
        public:
            WdeDefaultGraphicPipeline() = default;
            ~WdeDefaultGraphicPipeline() = default;

            // Disable copys
            WdeDefaultGraphicPipeline(const WdeDefaultGraphicPipeline&) = delete;
            WdeDefaultGraphicPipeline& operator=(const WdeDefaultGraphicPipeline&) = delete;

            /**
             * Initialize the rendering pipeline
             * @param shaderVertLocation The local path of the vertex shader
             * @param shaderFragLocation The local path of the fragment shader
             */
            void initialize(std::string &shaderVertLocation, std::string &shaderFragLocation);


        private:
            /** Create the class graphics pipeline */
            void createGraphicsPipeline(std::string &shaderVertLocation, std::string &shaderFragLocation);


            // Helper functions
            static std::vector<char> readFile(const std::string &fileName);
    };
}
