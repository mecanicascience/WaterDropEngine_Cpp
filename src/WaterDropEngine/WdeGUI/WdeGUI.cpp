#include "WdeGUI.hpp"
#include "../WaterDropEngine.hpp"

namespace wde::gui {
#ifdef WDE_ENGINE_MODE_DEBUG // Debug
	// Module commands
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)), Subject("GUI Subject") {
		// === Create ImGui context ===
		IMGUI_CHECKVERSION();
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initializing GUI Engine ==" << logger::endl;
		ImGui::CreateContext();

		// Setup ImGui config
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Setting up gui configuration." << logger::endl;
		// Enable windows docking and keyboard
		ImGuiIO& io = ImGui::GetIO(); (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Broken for now for Vulkan support

		// Setup fonts (using Raleway)
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Setting up gui fonts." << logger::endl;
		/*io.Fonts->AddFontFromFileTTF("res/fonts/opensans/OpenSans-Bold.ttf", 15.0f);
		 * io.Fonts->AddFontFromFileTTF("res/fonts/opensans/OpenSans-SemiBold.ttf", 15.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/opensans/OpenSans-Regular.ttf", 15.0f);*/
		io.Fonts->AddFontFromFileTTF("res/fonts/raleway/static/Raleway-Bold.ttf", 16.0f);
		io.Fonts->AddFontFromFileTTF("res/fonts/raleway/static/Raleway-SemiBold.ttf", 16.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/raleway/static/Raleway-Regular.ttf", 16.0f);

		// Dark mode
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Setting up gui color scheme." << logger::endl;
		GUITheme::setCustomColorTheme();
		GUITheme::setCustomStyle();

		// Use classic windows theme on Windows
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// === Initialize ImGui for GLFW ===
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Configuring ImGUI with GLFW." << logger::endl;
		auto& renderInstance = WaterDropEngine::get().getRender();
		ImGui_ImplGlfw_InitForVulkan(&renderInstance.getWindow().getWindow(), true);

		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initialization Done ==" << logger::endl;
	}

	void WdeGUI::initialize(std::pair<int, int> renderStage) {
		auto& renderInstance = WaterDropEngine::get().getRender();
		WaterDropEngine::get().getGUI()._initialized = true;

		// === Initialize ImGui for Vulkan ===
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = renderInstance.getInstance().getInstance();
		initInfo.PhysicalDevice = renderInstance.getInstance().getDevice().getPhysicalDevice();
		initInfo.Device = renderInstance.getInstance().getDevice().getDevice();

		// Using the same graphics queue as the render engine
		initInfo.QueueFamily = renderInstance.getInstance().getDevice().findQueueFamilies().graphicsFamily;
		initInfo.Queue = renderInstance.getInstance().getDevice().getGraphicsQueue();

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
		WaterDropEngine::get().getGUI()._imGUIdescriptorPool = std::make_shared<render::DescriptorPool>(poolSizes, poolMaxElements * 11, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
		initInfo.DescriptorPool = WaterDropEngine::get().getGUI()._imGUIdescriptorPool->getPool();

		// Continue parameters configuration
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.Allocator = nullptr;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = renderInstance.getInstance().getSwapchain().getImageCount();
		initInfo.CheckVkResultFn = OnGuiConfigError;
		initInfo.Subpass = renderStage.second;

		render::RenderPass& pass = WaterDropEngine::get().getInstance().getPipeline().getRenderPass(renderStage.first);

		// == Create ImGui render pass ==
		ImGui_ImplVulkan_Init(&initInfo, WaterDropEngine::get().getInstance().getPipeline().getRenderPass(renderStage.first).getRenderPass());

		// == Uploads ImGui fonts to the GPU ==
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Uploading ImGui fonts to the GPU." << logger::endl;
		render::CommandBuffer commandBuffer {false};
		commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		commandBuffer.end();
		commandBuffer.submit();
		renderInstance.getInstance().waitForDevicesReady();
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}


	void WdeGUI::tick() {
		WDE_PROFILE_FUNCTION();
		// Update gui bar
		_guiBar.updateGUI();
	}

	void WdeGUI::render(render::CommandBuffer &commandBuffer) {
		WDE_PROFILE_FUNCTION();

#ifdef WDE_ENGINE_MODE_PRODUCTION
		return;
#endif

		// Start the ImGui frame
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Rendering GUI new frame." << logger::endl;
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// RENDER
		// Setup main window
		ImGuiWindowFlags windowFlags =
				ImGuiWindowFlags_NoTitleBar               // No title bar
				| ImGuiWindowFlags_NoCollapse               // Doesn't allow collapsing
				| ImGuiWindowFlags_NoResize                 // Resize is handled by the program
				| ImGuiWindowFlags_NoMove                   // Cannot move window around
				| ImGuiWindowFlags_NoFocusOnAppearing       // Disable focus animation
				| ImGuiWindowFlags_NoBringToFrontOnFocus    // Disable focus grayish color
				| ImGuiWindowFlags_NoNavFocus               // Cannot focus window (auto-focused)
				| ImGuiWindowFlags_NoDocking                // Main CoreWindow cannot be docked
				| ImGuiWindowFlags_NoBackground             // Disables window background
				| ImGuiWindowFlags_MenuBar;                 // Enable menu bar

		// Set main viewport as fullscreen
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Main viewport not round, no border, and no padding
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str(), nullptr, windowFlags);
		ImGui::PopStyleVar(3);


		// ==== BUILD GUI ====
		if (ImGui::DockBuilderGetNode(ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str())) == nullptr) {
			logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Creating GUI elements." << logger::endl;
			ImGuiID dockspaceID = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
			ImGuiViewport *viewportGUI = ImGui::GetMainViewport();

			// Clear out the existing global layout
			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, viewportGUI->Size);

			// Create gui
			WaterDropEngine::get().getGUI().notify({LogChannel::GUI, "CreateGUI"});

			ImGui::DockBuilderFinish(dockspaceID);
		}

		// Setup GUI bar every frame
		ImGui::BeginMenuBar();
		WaterDropEngine::get().getGUI()._guiBar.renderGUI();
		ImGui::EndMenuBar();
		// ==== END BUILD GUI ====


		// Set dockspace main flags
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode; // No background
		ImGuiID dockspaceId = ImGui::GetID(WaterDropEngine::get().getGUI().DOCKSPACE_ROOT_ID.c_str());
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		// End dockspace
		ImGui::End();

		// ==== RENDER ELEMENTS HERE ====
		if (WaterDropEngine::get().getGUI()._guiBar.displayGUI()) {
			// Notify GUI drawing
			WaterDropEngine::get().getGUI().notify({LogChannel::GUI, "DrawGUI"});
		}

		// Rendering
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

		// Update windows size (currently broken for Vulkan in ImGui)
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Render GUI elements in Scene
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Rendering GUI elements." << logger::endl;
	}

	void WdeGUI::cleanUp() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up GUI Engine ==" << logger::endl;

		// Destroy descriptor
		_imGUIdescriptorPool.reset();

		// Clean Up Context
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Cleaning up Gui context." << logger::endl;
		if (WaterDropEngine::get().getGUI()._initialized) {
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up Done ==" << logger::endl;
	}

#endif

#ifdef WDE_ENGINE_MODE_PRODUCTION // Production
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)), Subject("GUI Subject") {}
	void WdeGUI::initialize(std::pair<int, int> renderStage) {}
	void WdeGUI::tick() {}
	void WdeGUI::cleanUp() {}
	void WdeGUI::render(render::CommandBuffer &commandBuffer) {}
#endif
}
