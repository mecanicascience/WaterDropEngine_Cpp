#include "../../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "PipelineExample04.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class EngineInstanceExample04 : public WdeInstance {
		public:
			void initialize() override {
				setRenderPipeline(std::make_shared<PipelineExample04>());
			}

			void update() override { }

			void cleanUp() override { }
	};
}
