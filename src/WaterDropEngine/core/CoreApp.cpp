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

        // Initialize the graphics renderer
        wdeRenderer.initialize();

        // Set references
        wdeSwapChain.setGraphicsPipeline(&wdeGraphicsPipeline);
        wdeSwapChain.setRenderer(&wdeRenderer);
        wdeWindow.setRenderer(&wdeRenderer);
    }

    void CoreApp::run() {
        std::cout << "Program started." << std::endl;
        while(!wdeWindow.shouldClose()) {
            glfwPollEvents(); // Extracts keystrokes, user click button, ...
            wdeRenderer.drawFrame(); // Draw next frame
        }

        vkDeviceWaitIdle(wdeInstanceDevice.getDevice()); // wait for app to finish drawing frame (because we use async funcs)
    }

    void CoreApp::cleanUp() {
        // wdeWindow deleted
    }
}
