#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::scene;

class WaterModule : public Module {
    public:
        explicit WaterModule(GameObject &gameObject, std::shared_ptr<Model> model, std::shared_ptr<Material> material)
                : _model(std::move(model)),
                  _material(std::move(material)),
                  _pipeline("res/shaders/compute/testcompute.comp.spv"),
                  Module(gameObject, "Water Module") {}

        // Core functions
        void createDescriptors() override;
        void initialize() override;
        void preRender(CommandBuffer& commandBuffer, RenderStage stage) override;
        void render(CommandBuffer& commandBuffer, RenderStage stage) override;
        void renderGUI() override;
        void cleanUp() override;


    private:
        std::shared_ptr<Model> _model;
        std::shared_ptr<Material> _material {};
        PipelineCompute _pipeline;

        std::shared_ptr<Descriptor> _computeDescriptor {};
};

