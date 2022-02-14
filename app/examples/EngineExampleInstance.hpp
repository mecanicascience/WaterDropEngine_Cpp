#include "../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "01-Triangle/PipelineExample01.hpp"
#include "02-3D_Cube/PipelineExample02.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class EngineExampleInstance : public WdeInstance {
		public:
			void initialize() override {
				// 01 - Triangle example
				//setRenderPipeline(std::make_unique<PipelineExample01>());

				// 02 - 3D Cube example
				setRenderPipeline(std::make_unique<PipelineExample02>());
			}

			void update() override { }

			void cleanUp() override { }
	};
}
