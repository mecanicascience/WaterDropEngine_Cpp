#pragma once

#include <utility>

#include "../Subrenderer.hpp"

namespace wde::renderEngine {
	/**
	 * Class representing a post processing filter.
	 * This filter assumes that the current sub-render-pass will import the given binding indices X, and
	 * the filter will give them to the shaders as set 0, binding X, attachment X
	 */
	class FilterSubrenderer : public Subrenderer {
		public:
			// Constructors
			/**
			 * Create the filter at the given stage
			 * @param stage
			 * @param filterShader The vertex shader of the filter
			 * @param inputAttachmentBinding The bindings of the attachments that the filter will read from (they will be in the same order on set 0)
			 */
			explicit FilterSubrenderer(const RenderStage &stage, const std::string& filterShader, std::vector<uint32_t> inputAttachmentBindings)
				: Subrenderer(stage), _inputAttachmentBindings(std::move(inputAttachmentBindings)),
				  _pipeline(stage, {"res/shaders/filters/defaultFilter.vert.spv", "res/shaders/filters/" + filterShader},
						  {}, PipelineGraphics::Mode::MRT, PipelineGraphics::Depth::ReadWrite,
						  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_POLYGON_MODE_FILL,
						  VK_CULL_MODE_NONE) {}


			// Core functions
			/** Initialize the sub-renderer pipeline and descriptor */
			void initialize() override {
				WDE_PROFILE_FUNCTION();
				// Initialize descriptor
				_descriptor = std::make_shared<Descriptor>();

				// Set descriptor bindings inputs
				std::vector<DescriptorSetBindingData> bindingData {};
				for (uint32_t i = 0; i < _inputAttachmentBindings.size(); i++)
					bindingData.push_back({
						i,
						VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
						_inputAttachmentBindings[i],
						_stage.first
					});

				_descriptor->addSet(0, bindingData);

				// Setup pipeline
				_pipeline.setDescriptor(_descriptor);
				_pipeline.initialize();
			}

			/** Renders the filter */
			void render(CommandBuffer &commandBuffer) override {
				WDE_PROFILE_FUNCTION();
				Logger::debug("Rendering filter at stage " + std::to_string(_stage.first)
					+ "-" + std::to_string(_stage.second) + ".", LoggerChannel::RENDERING_ENGINE);

				// Bind pipeline
				_pipeline.bind(commandBuffer);
				_pipeline.bind(_descriptor);

				// Render filter
				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
			}


		protected:
			/** The associated filter render pipeline */
			PipelineGraphics _pipeline;
			/** The associated filter ressource descriptor */
			std::shared_ptr<Descriptor> _descriptor;

			/** The attachment binding index */
			std::vector<uint32_t> _inputAttachmentBindings;
	};
}

