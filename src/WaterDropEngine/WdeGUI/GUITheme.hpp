#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUITheme {
		public:
			// Main theme colors
			const inline static ImVec4 colorMajor {0.06f, 0.06f, 0.06f, 1.0f}; // Background header color
			const inline static ImVec4 colorMajorLight {0.12f, 0.12f, 0.12f, 1.0f};
			const inline static ImVec4 colorMinor {0.17f, 0.17f, 0.17f, 1.0f}; // Background content color
			const inline static ImVec4 colorMinorLight {0.25f, 0.25f, 0.25f, 1.0f};
			const inline static ImVec4 colorMinorLightLight {0.35f, 0.35f, 0.35f, 1.0f};
			const inline static ImVec4 colorMinorLightLightLight {0.45f, 0.45f, 0.45f, 1.0f};

			// Text color
			const inline static ImVec4 colorTextMajor {0.823, 0.823, 0.823, 1.0f};
			const inline static ImVec4 colorTextMinor {0.78, 0.78, 0.78, 1.0f};


			// Black and white colors
			const inline static ImVec4 colorWhiteMajor {1.0f, 1.0f, 1.0f, 1.0f};
			const inline static ImVec4 colorWhiteMinor {0.93f, 0.93f, 0.93f, 1.0f};

			const inline static ImVec4 colorGrayMajor {0.86f, 0.86f, 0.86f, 1.0f};
			const inline static ImVec4 colorGrayMinor {0.47f, 0.47f, 0.47f, 1.0f};

			const inline static ImVec4 colorBlackMajor {0.0f, 0.0f, 0.0f, 1.0f};
			const inline static ImVec4 colorBlackMinor {0.2f, 0.2, 0.2f, 1.0f};


			// RGB full colors
			const inline static ImVec4 colorRedMajor {0.87f, 0.14f, 0.11f, 1.0f};
			const inline static ImVec4 colorRedMinor {0.98f, 0.52f, 0.63f, 1.0f};

			const inline static ImVec4 colorOrangeMajor {0.96f, 0.58f, 0.19f, 1.0f};
			const inline static ImVec4 colorOrangeMinor {0.96f, 0.77f, 0.45f, 1.0f};

			const inline static ImVec4 colorGreenMajor {0.12f, 0.85f, 0.26f, 1.0f};
			const inline static ImVec4 colorGreenMinor {0.6f, 0.98f, 0.52f, 1.0f};

			const inline static ImVec4 colorPurpleMajor {0.6f, 0.27f, 1.0f, 1.0f};
			const inline static ImVec4 colorPurpleMinor {0.77f, 0.58f, 1.0f, 1.0f};

			const inline static ImVec4 colorBlueMajor {0.18f, 0.33f, 1.0f, 1.0f};
			const inline static ImVec4 colorBlueMinor {0.52f, 0.6f, 1.0f, 1.0f};


			/**
			 * Sets the custom ImGUI color theme
			 */
			static void setCustomColorTheme() {
				WDE_PROFILE_FUNCTION();
				ImGui::StyleColorsDark(); // Use dark theme as base

				auto& colors = ImGui::GetStyle().Colors;

				// Background color
				colors[ImGuiCol_Text] = colorTextMajor;
				colors[ImGuiCol_WindowBg] = colorMinor;
				colors[ImGuiCol_ChildBg] = colorMinor;
				colors[ImGuiCol_SeparatorActive] = colorMinorLightLightLight;
				colors[ImGuiCol_SeparatorHovered] = colorMinorLightLight;

				// CoreWindow headers
				colors[ImGuiCol_TitleBg] = colorMajor;
				colors[ImGuiCol_TitleBgActive] = colorMajor;
				colors[ImGuiCol_TitleBgCollapsed] = colorMajor;

				// Top menu
				colors[ImGuiCol_MenuBarBg] = colorMajor;

				// Tabs (window header components)
				colors[ImGuiCol_Tab] = colorMajorLight;
				colors[ImGuiCol_TabHovered] = colorMinorLight;
				colors[ImGuiCol_TabActive] = colorMinor;
				colors[ImGuiCol_TabUnfocused] = colorMajorLight;
				colors[ImGuiCol_TabUnfocusedActive] = colorMinor;

				// Buttons
				colors[ImGuiCol_Button] = colorMinorLight;
				colors[ImGuiCol_ButtonHovered] = colorMinor;
				colors[ImGuiCol_ButtonActive] = colorMajor;
				colors[ImGuiCol_CheckMark] = colorMajor;

				// Frame BG
				colors[ImGuiCol_FrameBg] = colorMinorLight;
				colors[ImGuiCol_FrameBgHovered] = colorMinorLight;
				colors[ImGuiCol_FrameBgActive] = colorMinorLight;

				// Headers (window children titles and select color)
				colors[ImGuiCol_Header] = colorMinorLightLight;
				colors[ImGuiCol_HeaderHovered] = colorMinorLightLightLight;
				colors[ImGuiCol_HeaderActive] = colorMinorLightLightLight;
			}


			/**
			 * Sets the custom ImGUI configuration style
			 */
			static void setCustomStyle() {
				WDE_PROFILE_FUNCTION();
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
				style.FramePadding = ImVec2(10, 4);

				// Setup popup style
				style.PopupBorderSize = 0;
				style.PopupRounding = 0;

				// Setup child style
				style.ChildBorderSize = 0;
				style.ChildRounding = 0;

				// Setup popup style
				style.PopupBorderSize = 0;
				style.PopupRounding = 0;

				// Setup scrollbar style
				style.ScrollbarSize = 14;
				style.ScrollbarRounding = 0;

				// Setup items style
				style.ItemSpacing = ImVec2(10, 4);
				style.ItemInnerSpacing = ImVec2(6, 4);
				style.SelectableTextAlign = ImVec2(0.02f, 0);
			}
		};
}

