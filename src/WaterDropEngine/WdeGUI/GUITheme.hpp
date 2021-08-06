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


			// Main Wde colors
			const inline static ImVec4 colorWdeMajor {0.69f, 0.92f, 0.99f, 1.0f};
			const inline static ImVec4 colorWdeMinor {0.83f, 0.92f, 0.96f, 1.0f};
			const inline static ImVec4 colorWdeDarkMajor {0.46f, 0.58f, 0.89f, 1.0f};
			const inline static ImVec4 colorWdeDarkMinor {0.33f, 0.42f, 0.68f, 1.0f};


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
				ImGui::StyleColorsDark(); // Use dark theme as base

				auto& colors = ImGui::GetStyle().Colors;

				// Background color
				colors[ImGuiCol_WindowBg] = colorMinor;

				// Window headers
				colors[ImGuiCol_TitleBg] = colorMajor;
				colors[ImGuiCol_TitleBgActive] = colorMajor;
				colors[ImGuiCol_TitleBgCollapsed] = colorMajor;

				// Top menu
				colors[ImGuiCol_MenuBarBg] = colorMajor;

				// Tabs (window header components)
				colors[ImGuiCol_Tab] = colorMinor;
				colors[ImGuiCol_TabHovered] = colorMinor;
				colors[ImGuiCol_TabActive] = colorMinor;
				colors[ImGuiCol_TabUnfocused] = colorMinor;
				colors[ImGuiCol_TabUnfocusedActive] = colorMinor;

				// Buttons
				colors[ImGuiCol_Button] = colorMinorLight;
				colors[ImGuiCol_ButtonHovered] = colorMinorLight;
				colors[ImGuiCol_ButtonActive] = colorMinorLight;

				// Frame BG
				colors[ImGuiCol_FrameBg] = colorMinorLight;
				colors[ImGuiCol_FrameBgHovered] = colorMinorLight;
				colors[ImGuiCol_FrameBgActive] = colorMinorLight;

				// Headers (window children titles and select color)
				colors[ImGuiCol_Header] = colorWdeDarkMinor;
				colors[ImGuiCol_HeaderHovered] = colorWdeDarkMajor;
				colors[ImGuiCol_HeaderActive] = colorWdeDarkMajor;
			}


			/**
			 * Sets the custom ImGUI configuration style
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
			}
	};
}

