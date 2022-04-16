#pragma once

#include "../../wde.hpp"
#include "../WdeCommon/WdeFiles/WdeFileUtils.hpp"

namespace wde::gui {
	class GUIBar : public NonCopyable {
		public:
			/** Renders the gui top menu */
			void renderMenu();

			// Getters and setters
			bool displayGUI() const { return _displayGUI; }


		private:
			/** Display GUI by default */
			bool _displayGUI = true;
	};
}

