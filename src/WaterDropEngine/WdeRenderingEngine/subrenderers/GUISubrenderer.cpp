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
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = renderInstance.getInstance();
		initInfo.PhysicalDevice = renderInstance.getSelectedDevice().getPhysicalDevice();
		initInfo.Device = renderInstance.getSelectedDevice().getDevice();

		// Using the same graphics queue as the render engine
		initInfo.QueueFamily = renderEngine::CoreDeviceHelper::findQueueFamilies(renderInstance.getSelectedDevice().getPhysicalDevice()).graphicsFamily;
		initInfo.Queue = renderInstance.getSelectedDevice().getGraphicsQueue();

		// Create descriptor pool for ImGui
		uint32_t poolMaxElements = 1000;
		std::vector<VkDescriptorPoolSize> poolSizes = {
				{ VK_DESCRIPTOR_TYPE_SAMPLER,  poolMaxElements},
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, poolMaxElements },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, poolMaxElements }
		};
		_imGUIdescriptorPool = std::make_shared<DescriptorPool>(poolSizes, poolMaxElements * 11, false);
		_imGUIdescriptorPool->createPool(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
		initInfo.DescriptorPool = _imGUIdescriptorPool->getPool();

		// Continue parameters configuration
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.Allocator = nullptr;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = renderInstance.getSelectedDevice().getSwapChain().getImageCount();
		initInfo.CheckVkResultFn = OnGuiConfigError;
		initInfo.Subpass = stage.second;

		// == Create ImGui render pass ==
		ImGui_ImplVulkan_Init(&initInfo, *renderInstance.getRenderer()->getRenderPass(stage.first)->getRenderPass());

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

		// Destroy ImGUI
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


	// Render frame
	void GUISubrenderer::render(CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();
		// Start the ImGui frame
		Logger::debug("Rendering GUI new frame.", LoggerChannel::GUI);
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
