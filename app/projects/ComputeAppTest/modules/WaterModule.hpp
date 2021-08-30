#pragma once

#include <WaterDropEngine/includes.hpp>
#include "../materials/WaterMaterialCompute.hpp"

using namespace wde::scene;

class WaterModule : public Module {
    public:
		explicit WaterModule(GameObject &gameObject, int planeSizeFactor, std::shared_ptr<Model> model, std::shared_ptr<WaterMaterialCompute> material);

        // Core functions
        void createDescriptors() override;
        void initialize() override;
        void render(CommandBuffer& commandBuffer, RenderStage stage) override;
        void cleanUp() override;


    private:
		struct BufferData {
			float wavesHeight[32*32*10*10]; // Wave height
		};

		struct ReadBufferData {
			float time;
		};

		std::shared_ptr<Buffer> wavesData {};
		int _planeSizeFactor;

        std::shared_ptr<Model> _model;
        std::shared_ptr<WaterMaterialCompute> _material {};

        PipelineCompute _computePipeline;
        std::shared_ptr<Descriptor> _computeDescriptor {};
};

