#include "../../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "../02-3D_Cube/PipelineExample02.hpp"

#include "PipelineExample02.hpp"
#include "SceneExample02.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class EngineInstanceExample02 : public WdeInstance {
		public:
			void initialize() override {
				setRenderPipeline(std::make_unique<PipelineExample02>());
				setScene(std::make_unique<SceneExample02>());
			}

			void update() override { }

			void cleanUp() override { }
	};
}
