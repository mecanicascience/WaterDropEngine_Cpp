#pragma once

#include "Module.hpp"

namespace wde::scene {
	class DummyModule : public Module {
		public:
			explicit DummyModule() : Module("Dummy Module") {}

			void renderGUI() override {
				ImGui::Text("test");
			}
	};
}

