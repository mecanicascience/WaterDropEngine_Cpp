#include "../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "../src/WaterDropEngine/WaterDropEngine.hpp"
#include "../src/WaterDropEngine/WdeRender/render/passes/RenderPass.hpp"
#include "render/EngineRenderPipeline.hpp"

using namespace wde;
using namespace wde::render;

class EngineInstance : public WdeInstance {
	public:
		void initialize() override {
			// Set engine rendering pipeline
			setRenderPipeline(std::make_unique<EngineRenderPipeline>());
		}

		void update() override { }

		void cleanUp() override { }
};
