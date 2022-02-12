#include "../src/WaterDropEngine/WdeCore/Core/WaterDropEngineInstance.hpp"
#include "../src/WaterDropEngine/WaterDropEngine.hpp"

using namespace wde;
using namespace wde::render;

class EngineInstance : public WaterDropEngineInstance {
	public:
		void initialize() override { }

		void update() override { }

		void render(CommandBuffer &commandBuffer) override {
			CoreInstance& renderer = WaterDropEngine::get().getRender().getInstance();

			// For now, just change swapchain image layout to presentation layout
			VkImageMemoryBarrier barrier {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier.image = renderer.getSwapchain().getActiveImage();
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			// Execute command
			vkCmdPipelineBarrier(
					commandBuffer,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &barrier); // Use only memory barrie
		}

		void cleanUp() override { }
};
