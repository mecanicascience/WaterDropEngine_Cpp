#include "WdeGUI.hpp"

namespace wde::gui {
#if WDE_ENGINE_MODE == 2 // Debug
	// Module commands
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		WDE_PROFILE_FUNCTION();
		// TODO Create GUI
		return;

		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initializing GUI Engine ==" << logger::endl;
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Setting up gui configuration." << logger::endl;

		// Setup ImGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

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

		// Use classic windows theme on CoreWindow
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Initialization Done ==" << logger::endl;
	}

	void WdeGUI::tick() {
		WDE_PROFILE_FUNCTION();
		return;

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
		ImGui::Begin(DOCKSPACE_ROOT_ID.c_str(), nullptr, windowFlags);
		ImGui::PopStyleVar(3);

		// Set dockspace main flags
		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode; // No background
		ImGuiID dockspaceId = ImGui::GetID(DOCKSPACE_ROOT_ID.c_str());
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

		// End dockspace
		ImGui::End();

		// Render GUI elements in Scene
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Rendering GUI elements." << logger::endl;
	}

	void WdeGUI::cleanUp() {
		WDE_PROFILE_FUNCTION();
		return;

		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up GUI Engine ==" << logger::endl;

		// Clean Up
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Destroying ImGUI context." << logger::endl;
		ImGui::DestroyContext();

		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "== Cleaning Up Done ==" << logger::endl;
	}

	void WdeGUI::createElements() {
		logger::log(LogLevel::DEBUG, LogChannel::GUI) << "Creating GUI elements." << logger::endl;
		ImGuiID dockspaceID = ImGui::GetID(DOCKSPACE_ROOT_ID.c_str());
		ImGuiViewport* viewport = ImGui::GetMainViewport();

		// Clear out the existing global layout
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);
		ImGuiID dockparentID = dockspaceID; // Master node ID

		// End building
		ImGui::DockBuilderFinish(dockspaceID);
	}
#endif

#if WDE_ENGINE_MODE == 1 // Production
	WdeGUI::WdeGUI(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {}
	void WdeGUI::tick() {}
	void WdeGUI::cleanUp() {}
	void WdeGUI::createElements() {}
#endif
}
