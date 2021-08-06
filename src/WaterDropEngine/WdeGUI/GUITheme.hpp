#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUITheme {
		public:
			/**
			 * Sets the custom ImGUI color theme
			 */
			static void setCustomColorTheme() {
				ImGui::StyleColorsDark(); // Use dark theme as base

				auto& colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

				// Headers
				colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Buttons
				colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Frame BG
				colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
				colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
				colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

				// Tabs
				colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
				colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
				colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

				// Title
				colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
				colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			}


			/**
			 * Sets the custom ImGUI color style
			 */
			static void setCustomStyle() {
				ImGuiStyle& style = ImGui::GetStyle();

				// Setup window style
				style.WindowBorderSize = 0;
				style.WindowRounding = 0;
				style.WindowMenuButtonPosition = ImGuiDir_None; // Disable menu popup
				style.WindowTitleAlign = ImVec2(0.05, 0.5);
				style.WindowPadding = ImVec2(8, 8);

				// Setup frame style
				style.FrameRounding = 0;
				style.FrameBorderSize = 0;
				style.FramePadding  = ImVec2(10, 4);

				// Setup popup style
				style.PopupBorderSize = 0;
				style.PopupRounding = 0;

				// Setup child style
				style.ChildBorderSize = 0;
				style.ChildRounding = 0;

				// Setup scrollbar style
				style.ScrollbarSize = 14;
				style.ScrollbarRounding = 0;

				// Setup items style
				style.ItemSpacing = ImVec2(10, 4);
				style.ItemInnerSpacing = ImVec2(6, 4);
			}
	};
}

