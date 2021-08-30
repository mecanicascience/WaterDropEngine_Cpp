#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::scene;

class WaterModule : public Module {
    public:
        explicit WaterModule(GameObject &gameObject, std::shared_ptr<Model> model, std::shared_ptr<Material> material);

        // Core functions
        void createDescriptors() override;
        void initialize() override;
        void render(CommandBuffer& commandBuffer, RenderStage stage) override;
        void cleanUp() override;


    private:
        std::shared_ptr<Model> _model;
        std::shared_ptr<Material> _material {};

        PipelineCompute _computePipeline;
        std::shared_ptr<Descriptor> _computeDescriptor {};
};

