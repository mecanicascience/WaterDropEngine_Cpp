#pragma once

#include "../src/WaterDropEngine/WaterDropEngine.hpp"

using namespace wde;
class CoreApp {
    public:
        /** Create a new application */
        CoreApp() = default;

        /**
         * Initialize the application
         * @return An execution status value
         */
        WdeStatus initialize();


    private:
        WaterDropEngine engine {};
};
