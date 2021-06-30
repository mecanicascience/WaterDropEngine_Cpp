#pragma once

#include "WdeWindow.hpp"
#include "WdeInstanceDevice.hpp"
#include "../rendering/WdeSwapChain.hpp"
#include "../rendering/WdeDefaultGraphicsPipeline.hpp"

namespace wde {
    class CoreApp {
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
            WdeSwapChain wdeSwapChain {wdeInstanceDevice, wdeWindow};
            WdeDefaultGraphicsPipeline wdeGraphicsPipeline {wdeInstanceDevice, wdeSwapChain};
    };
}
