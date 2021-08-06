#include "WdeGUI.hpp"

namespace wde::gui {
	// ImGui sub-renderer functions
	void WdeGUI::setup() {
		WDE_PROFILE_FUNCTION();
		Logger::debug("Setting up gui configuration.", LoggerChannel::GUI);
		// Enable windows docking and keyboard
		ImGuiIO& io = ImGui::GetIO(); (void) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Broken for now for Vulkan support

		// Setup fonts (using Raleway)
		Logger::debug("Setting up gui fonts.", LoggerChannel::GUI);
		/*io.Fonts->AddFontFromFileTTF("res/fonts/opensans/OpenSans-Bold.ttf", 15.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/opensans/OpenSans-Regular.ttf", 15.0f);*/
		io.Fonts->AddFontFromFileTTF("res/fonts/raleway/static/Raleway-Bold.ttf", 16.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/raleway/static/Raleway-Regular.ttf", 16.0f);

		// Dark mode
		Logger::debug("Setting up gui color scheme.", LoggerChannel::GUI);
		GUITheme::setCustomColorTheme();
		GUITheme::setCustomStyle();

		// Use classic windows theme on Windows
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	void WdeGUI::render() {
		WDE_PROFILE_FUNCTION();

		// Setup main window
		ImGuiWindowFlags windowFlags =
				  ImGuiWindowFlags_NoTitleBar               // No title bar
				| ImGuiWindowFlags_NoCollapse               // Doesn't allow collapsing
				| ImGuiWindowFlags_NoResize                 // Resize is handled by the program
				| ImGuiWindowFlags_NoMove                   // Cannot move window around
				| ImGuiWindowFlags_NoFocusOnAppearing       // Disable focus animation
				| ImGuiWindowFlags_NoBringToFrontOnFocus    // Disable focus grayish color
				| ImGuiWindowFlags_NoNavFocus               // Cannot focus window (auto-focused)
				| ImGuiWindowFlags_NoDocking                // Main Window cannot be docked
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
		ImGui::Begin(DOCKSPACE_ROOT_ID.c_str(), nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		// Create or reset dockspace if needed
		if (_shouldResetElements || ImGui::DockBuilderGetNode(ImGui::GetID(DOCKSPACE_ROOT_ID.c_str())) == nullptr) {
			createElements();
			_shouldResetElements = false;
		}

		// Setup GUI bar every frame
		ImGui::BeginMenuBar();
		_guiBar.renderGUI();
		ImGui::EndMenuBar();

		// Set dockspace main flags
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode; // No background
		ImGuiID dockspaceId = ImGui::GetID(DOCKSPACE_ROOT_ID.c_str());
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		// End dockspace
		ImGui::End();

		// Render GUI elements in Scene
		Logger::debug("Rendering GUI elements.", LoggerChannel::GUI);
		scene::WdeSceneManager::get().renderGUI();
		_logger.renderGUI();
	}


	void WdeGUI::createElements() {
		WDE_PROFILE_FUNCTION();

		Logger::debug("Creating GUI elements.", LoggerChannel::GUI);
		ImGuiID dockspaceID = ImGui::GetID(DOCKSPACE_ROOT_ID.c_str());
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		// Clear out the existing global layout
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);
		ImGuiID dockparentID = dockspaceID; // Master node ID

		// Create windows
		scene::WdeSceneManager::get().setupGUI(dockparentID); // Update scene
		_logger.setupGUI(dockparentID); // Logger

		// End building
		ImGui::DockBuilderFinish(dockspaceID);
	}
}
