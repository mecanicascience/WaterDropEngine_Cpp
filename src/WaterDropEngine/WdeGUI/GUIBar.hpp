#pragma once

#include "../../wde.hpp"

namespace wde::gui {
	class GUIBar {
		public:
			/** Renders the gui top menu */
			void renderGUI();


			// Getters and setters
			bool displayGUI() { return _displayGUI; }


		private:
			/** Display GUI by default */
			bool _displayGUI = true;
	};
}

