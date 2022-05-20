#pragma once

#include "GUIPanel.hpp"
#include "../../WdeResourceManager/Resource.hpp"

namespace wde::gui {
	/**
	 * The GUI panel that draws resources list to the screen.
	 */
	class ResourcesPanel : public GUIPanel {
		private:
			void render() override;
	};
}
