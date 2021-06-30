#include "FirstApp.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    wde::FirstApp app {};

    try {
        std::cout << "=== Initializing program ===" << std::endl;
        app.init();

        std::cout << "=== Initialization done ===" << std::endl << std::endl << std::endl << "=== Starting program ===" << std::endl;
        app.run();

        std::cout << "=== Program ended ===" << std::endl << std::endl << std::endl << "=== Cleaning up ===" << std::endl;
        app.cleanUp();

        std::cout << "=== End of program ===" << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Exception caught : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

