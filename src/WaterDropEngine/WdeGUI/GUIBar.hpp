#pragma once

#include "../../wde.hpp"
#include "../WdeCommon/WdeFiles/WdeFileUtils.hpp"

namespace wde::gui {
	class GUIBar {
		public:
			/** Updates the gui top menu */
			void updateGUI();
			/** Renders the gui top menu */
			void renderGUI();

			// Getters and setters
			bool displayGUI() const { return _displayGUI; }


		private:
			/** Display GUI by default */
			bool _displayGUI = true;
			/** True if the user wants to reset the scene as soon as possible */
			bool _shouldLoadScene = false;
	};
}

