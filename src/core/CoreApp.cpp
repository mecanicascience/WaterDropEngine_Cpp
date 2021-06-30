#include "CoreApp.hpp"

namespace wde {
    void CoreApp::init() {
        std::string fragShaderLocation = "shaders/simpleShader.frag.spv";
        std::string vertShaderLocation = "shaders/simpleShader.vert.spv";

        // Initialize the GLFW window
        wdeWindow.initialize();

        // Initialize and connects devices to the GLFW window, creates data queues and creates debug layers
        wdeInstanceDevice.initialize();

        // Initialize swap queues, and handles images frames
        wdeSwapChain.initialize();

        // Initialize the main graphics pipeline
        wdeGraphicsPipeline.initialize(vertShaderLocation, fragShaderLocation);
    }

    void CoreApp::run() {
        std::cout << "Program started." << std::endl;
        while(!wdeWindow.shouldClose()) {
            wdeWindow.run(); // keystrokes, user click button, ...
        }
    }

    void CoreApp::cleanUp() {
        // wdeWindow deleted
    }
}
