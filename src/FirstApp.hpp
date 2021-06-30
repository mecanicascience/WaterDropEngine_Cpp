#pragma once

#include "engine/WdeWindow.hpp"
#include "engine/WdePipeline.hpp"
#include "engine/WdeInstanceDevice.hpp"

namespace wde {
    class FirstApp {
        public:
            /** Fixed window width */
            static constexpr int WIDTH = 800;

            /** Fixed window height */
            static constexpr int HEIGHT = 600;

            /** Initialize application */
            void init();

            /** Run the application */
            void run();

            /** Clean up everything else */
            void cleanUp();

        private:
            WdeWindow wdeWindow {WIDTH, HEIGHT, "WaterDrop Engine Window"};
            WdeInstanceDevice wdeInstanceDevice {wdeWindow};
            /* WdePipeline wdePipeline {
                wdeDevice,
                "shaders/simpleShader.vert.spv",
                "shaders/simpleShader.frag.spv",
                WdePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)
            };*/
    };
}
