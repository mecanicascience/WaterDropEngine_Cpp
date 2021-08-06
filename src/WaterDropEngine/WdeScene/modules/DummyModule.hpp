#pragma once

#include "Module.hpp"

namespace wde::scene {
	class DummyModule : public Module {
		public:
			explicit DummyModule() : Module("Dummy Module") {}

			void initialize() override {
				std::cout << "initialize" << std::endl;
			}

			void update(float deltaTime) override {
				std::cout << "update : " << deltaTime << std::endl;
			}

			void render(CommandBuffer& commandBuffer) override {
				std::cout << "render" << std::endl;
			}

			void cleanUp() override {
				std::cout << "cleanUp" << std::endl;
			}
	};
}

