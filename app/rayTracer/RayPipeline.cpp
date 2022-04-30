#include "RayPipeline.hpp"
#include "../../src/WaterDropEngine/WaterDropEngine.hpp"

void RayPipeline::setup() {
	// Create passes attachments
	setAttachments({
		{0, "Depth texture", RenderAttachment::DEPTH},
		{1, "Swapchain attachment", RenderAttachment::SWAPCHAIN, VK_FORMAT_UNDEFINED, Color(0.1f, 0.105f, 0.11f)}
	});

	// Create passes and subpasses structure
	setStructure({
		{0, {
			{0, {}, { 0, 1 }}, // Scene
			{1, {}, { 1 }} // GUI
		}}
	});

	// Initialize GUI
	gui::WdeGUI::initialize(std::pair<int, int>{0, 1});
}

void RayPipeline::render(CommandBuffer &commandBuffer, scene::WdeSceneInstance &scene) {
	beginRenderPass(0);
		beginRenderSubPass(0);
		endRenderSubPass();

		beginRenderSubPass(1);
			// Render GUI
			gui::WdeGUI::render(commandBuffer);
		endRenderSubPass();
	endRenderPass();
}

void RayPipeline::cleanUp() {
	// Clean up raytracer
	_image.reset();
}

void RayPipeline::onNotify(const core::Event& event) {
	#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "CreateGUI") {

		}

		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("RenderZone");
			_viewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
			_viewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);
			if (_image)
				ImGui::Image(_image->getDescriptorSet(), { (float)_image->getWidth(), (float)_image->getHeight() });

			ImGui::End();
			ImGui::PopStyleVar();
		}
	#endif
}

