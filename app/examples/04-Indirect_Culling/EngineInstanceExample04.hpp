#include "../../../src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp"
#include "PipelineExample04.hpp"
#include "SceneExample04.hpp"

using namespace wde;
using namespace wde::render;

namespace examples {
	class EngineInstanceExample04 : public WdeInstance {
		public:
			void initialize() override {
				setRenderPipeline(std::make_shared<PipelineExample04>());
				setScene(std::make_shared<SceneExample04>());
			}

			void update() override { }

			void cleanUp() override { }
	};
}
