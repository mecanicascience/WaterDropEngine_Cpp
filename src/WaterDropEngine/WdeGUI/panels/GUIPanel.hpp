#pragma once

#include "../../../wde.hpp"
#include "../GUITheme.hpp"

namespace wde::gui {
	class GUIPanel {
		public:
			// Core methods
			explicit GUIPanel() = default;
			~GUIPanel() = default;

			// GUI Panel methods
			/** Render the panel if active */
			void renderPanel() {
				if (_active)
					render();
			}

			// Update display
			bool& getActiveStatus() { return _active; }

		protected:
			/** Render the panel */
			virtual void render() = 0;

		private:
			bool _active = false;
	};
}
