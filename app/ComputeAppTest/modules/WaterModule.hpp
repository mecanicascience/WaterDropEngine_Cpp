#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde::scene;

class WaterModule : public Module {
    public:
        explicit WaterModule(GameObject &gameObject, std::shared_ptr<Model> model, std::shared_ptr<Material> material)
                : _model(std::move(model)),
                  _material(std::move(material)),
                  _computePipeline("res/shaders/compute/computeShaderChessboard.comp.spv"),
                  _textureDisplayPipeline({0, 0}, {"res/shaders/texture/imageDisplayer.vert.spv", "res/shaders/texture/imageDisplayer.frag.spv"}, {},
										  PipelineGraphics::Mode::Polygon, PipelineGraphics::Depth::ReadWrite,
										  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
										  VK_CULL_MODE_NONE),
                  Module(gameObject, "Water Module") {}

        // Core functions
        void createDescriptors() override;
        void initialize() override;
        void render(CommandBuffer& commandBuffer, RenderStage stage) override;
        void renderGUI() override;
        void cleanUp() override;


    private:
        std::shared_ptr<Model> _model;
        std::shared_ptr<Material> _material {};

        std::shared_ptr<Texture2D> _chessboardImage {};

        PipelineCompute _computePipeline;
        std::shared_ptr<Descriptor> _computeDescriptor {};

        PipelineGraphics _textureDisplayPipeline;
        std::shared_ptr<Descriptor> _textureDisplayDescriptor {};
};

