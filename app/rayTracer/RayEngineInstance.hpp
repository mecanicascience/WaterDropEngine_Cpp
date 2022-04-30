#pragma once

#include "../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "RayPipeline.hpp"

class RayEngineInstance : public wde::WdeInstance {
	public:
		void initialize() override {
			setRenderPipeline(std::make_shared<RayPipeline>());
		}

		void update() override { }

		void cleanUp() override { }
};

