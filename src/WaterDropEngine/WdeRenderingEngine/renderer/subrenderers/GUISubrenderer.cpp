#include "GUISubrenderer.hpp"

namespace wde::renderEngine {
	/**
	 * On a ImGui initialization error is thrown
	 * @param err
	 */
	static void OnGuiConfigError(VkResult err) {
		if (err == 0)
			return;
		Logger::err("ImGui reported an error :" + std::to_string(err), LoggerChannel::GUI);
	}

	GUISubrenderer::GUISubrenderer(const RenderStage &stage) : Subrenderer(stage) {
		WDE_PROFILE_FUNCTION();
		Logger::debug("== Initializing GUI ==", LoggerChannel::GUI);
		// === Create ImGui context ===
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup ImGui config
		gui::WdeGUI::get().setup();

		// === Initialize ImGui for GLFW ===
		Logger::debug("Configuring ImGUI with GLFW.", LoggerChannel::GUI);
		auto& renderInstance = WdeRenderEngine::get();
		ImGui_ImplGlfw_InitForVulkan(renderInstance.getWindow(), true);


		// === Initialize ImGui for Vulkan ===
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = renderInstance.getInstance();
		init_info.PhysicalDevice = renderInstance.getSelectedDevice().getPhysicalDevice();
		init_info.Device = renderInstance.getSelectedDevice().getDevice();

		// Using the same graphics queue as the render engine
		init_info.QueueFamily = renderEngine::CoreDeviceHelper::findQueueFamilies(renderInstance.getSelectedDevice().getPhysicalDevice()).graphicsFamily;
		init_info.Queue = renderInstance.getSelectedDevice().getGraphicsQueue();

		// Create descriptor pool for ImGui
		VkDescriptorPoolSize pool_sizes[] = { // TODO implement a render descriptor pool creation option
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if(vkCreateDescriptorPool(renderInstance.getSelectedDevice().getDevice(), &pool_info, nullptr, &_descriptorPool) != VK_SUCCESS)
			throw WdeException("Failed to create a descriptor pool for the ImGUI.", LoggerChannel::GUI);
		init_info.DescriptorPool = _descriptorPool;

		// Continue parameters configuration
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = renderInstance.getSelectedDevice().getSwapChain().getImageCount();
		init_info.CheckVkResultFn = OnGuiConfigError;

		// == Create ImGui render pass ==
		ImGui_ImplVulkan_Init(&init_info, *renderInstance.getRenderer()->getRenderPass(stage.first)->getRenderPass());

		// == Uploads ImGui fonts to the GPU ==
		Logger::debug("Uploading ImGui fonts to the GPU.", LoggerChannel::GUI);
		renderEngine::CommandBuffer commandBuffer {false};
		commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		commandBuffer.end();
		commandBuffer.submit();
		renderInstance.waitForDevicesReady();
		ImGui_ImplVulkan_DestroyFontUploadObjects();

		Logger::debug("== Initialization Done ==", LoggerChannel::GUI);
	}

	GUISubrenderer::~GUISubrenderer() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Cleaning up Gui sub-renderer.", LoggerChannel::GUI);
		// Destroy command pool
		vkDestroyDescriptorPool(WdeRenderEngine::get().getSelectedDevice().getDevice(), _descriptorPool, nullptr);

		// Destroy ImGUI
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	// Render frame
	void GUISubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		// Start the ImGui frame
		Logger::debug("Rendering UI new frame.", LoggerChannel::GUI);
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Renders GUI
		gui::WdeGUI::get().render();

		// Rendering
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

		// Update windows size (currently broken for Vulkan in ImGui)
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}
