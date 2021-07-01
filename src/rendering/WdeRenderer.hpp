#pragma once

#include "../core/WdeWindow.hpp"
#include "../core/WdeInstanceDevice.hpp"
#include "WdeSwapChain.hpp"
#include "WdePipeline.hpp"

namespace wde {
    class WdeWindow;

    class WdeRenderer {
        public:
            WdeRenderer(WdeWindow &window, WdeInstanceDevice &device, WdeSwapChain &swapChain, WdePipeline &pipeline)
                : window{window}, device{device}, swapChain{swapChain}, pipeline{pipeline} {};
            ~WdeRenderer();

            // Avoid copys
            WdeRenderer(const WdeRenderer &) = delete;
            WdeRenderer &operator=(const WdeRenderer &) = delete;

            // Setters
            void shouldResizeFrameBuffer() { this->framebufferResized = true; }

            /** Initialize the renderer */
            void initialize();

            /** Draws the next frame */
            void drawFrame();


            /** Initialize the command buffers (this->commandBuffers) */
            void createCommandBuffers();
            /** Empty the command buffers */
            void clearCommandBuffers();


        private:
            /** Max frames count that can be processed at the same time */
            const int MAX_FRAMES_IN_FLIGHT = 2;

            /** true if the frame buffer should be resized */
            bool framebufferResized = false;

            WdeWindow &window;
            WdeInstanceDevice &device;
            WdeSwapChain &swapChain;
            WdePipeline &pipeline;

            /** Manage memory that is used to store buffers - command buffers allocated from them */
            VkCommandPool commandPool;
            /** The command buffers (one for each VkFrameBuffer - auto submits to command queues for processing) */
            std::vector<VkCommandBuffer> commandBuffers;

            /** Semaphores for when an image is available (id = frame id to be rendered) */
            std::vector<VkSemaphore> imageAvailableSemaphores;
            /** Semaphores for when an image is done rendering (id = frame id to be rendered) */
            std::vector<VkSemaphore> renderFinishedSemaphores;
            /** Fences for when GPU is ready to be called by CPU (we wait for fences to continue) */
            std::vector<VkFence> inFlightFences;
            /** Fences for which swap chain images are already being processed */
            std::vector<VkFence> imagesInFlight;

            /** The current frame for the semaphores to use right pair */
            size_t currentFrame = 0;


            /** Creates the command pool (this->commandPool) */
            void createCommandPool();

            /** Create semaphores for operations sync */
            void createSyncObjects();
    };
}
