#include "../../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "PipelineExample05.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class EngineInstanceExample05 : public WdeInstance {
		public:
			void initialize() override {
				//setRenderPipeline(std::make_shared<PipelineExample05>());
			}

			void update() override { }

			void cleanUp() override { }
	};
}
