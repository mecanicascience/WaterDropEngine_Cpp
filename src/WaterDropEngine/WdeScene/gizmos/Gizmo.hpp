#pragma once

#include <vector>

#include "../../../wde.hpp"

namespace wde::scene {
	class Gizmo {
		public:
			Gizmo() = default;

			// Drawing functions
			void drawLine(glm::vec3 from, glm::vec3 to) {
				_lines.emplace_back(from, to);
			}


		private:
			std::vector<std::pair<glm::vec3, glm::vec3>> _lines {};
	};
}
