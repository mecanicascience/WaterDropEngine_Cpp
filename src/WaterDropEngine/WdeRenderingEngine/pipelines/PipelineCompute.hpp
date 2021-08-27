#include "../../../wde.hpp"
#include "Pipeline.hpp"

namespace wde::renderEngine {
    class PipelineCompute : public Pipeline {
        public:
            // Constructors
            explicit PipelineCompute(std::string shaderStage)
                : _shaderStage(std::move(shaderStage)), _pipelineBindPoint(VK_PIPELINE_BIND_POINT_COMPUTE) {}
            ~PipelineCompute() override;

            // Core functions
            /** Initialize the compute pipeline */
            void initialize() override;


            // Getters and setters
            const VkPipeline &getPipeline() const override { return _pipeline; };
            const VkPipelineBindPoint &getPipelineBindPoint() const override { return _pipelineBindPoint; };

        private:
            // Vulkan parameters
            /** Type of the pipeline (compute) */
            VkPipelineBindPoint _pipelineBindPoint;

            // Shader components
            /** The path of the shader */
            std::string _shaderStage;
            /** Shader module description */
            VkPipelineShaderStageCreateInfo _shaderDescription {};
            /** The instance of the shader module */
            VkShaderModule _shaderModule {};


            // Core functions
            void createShaderModule();
            void createPipelineLayout();
            void createPipeline();
    };
}
