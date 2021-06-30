#include "FirstApp.hpp"

namespace wde {
    void FirstApp::init() {
        // Initialize the GLFW window
        wdeWindow.initialize();

        // Initialize and connects devices to the GLFW window, creates data queues and creates debug layers
        wdeInstanceDevice.initialize();
    }

    void FirstApp::run() {
        std::cout << "Program started." << std::endl;
        while(!wdeWindow.shouldClose()) {
            wdeWindow.run(); // keystrokes, user click button, ...
        }
    }

    void FirstApp::cleanUp() {
        // wdeWindow deleted
    }
}
