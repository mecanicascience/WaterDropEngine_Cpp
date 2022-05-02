#include "RayPipeline.hpp"
#include "../../src/WaterDropEngine/WaterDropEngine.hpp"

void RayPipeline::renderRayTracing() {
	if (_imageData == nullptr)
		return;

	// Get camera
	auto cam = WaterDropEngine::get().getInstance().getScene()->getActiveCamera();
	if (cam == nullptr)
		return;
	auto camMod = cam->getModule<scene::CameraModule>();
	if (camMod == nullptr)
		return;
	auto aspectRatio = camMod->getAspect();
	float rayLength = camMod->getFar() - camMod->getNear();

	// Do ray tracing
	auto uvSinX = std::sin(glm::radians(camMod->getFov() * aspectRatio) / 2);
	auto uvSinY = std::sin(glm::radians(camMod->getFov()) / 2);
	for (int j = _viewportHeight - 1; j >= 0; j--) {
		for (int i = 0; i < _viewportWidth; i++) {
			glm::vec2 uv {
				float(i) / static_cast<float>(_viewportWidth - 1),
				float(_viewportHeight - 1 - j) / static_cast<float>(_viewportHeight - 1)
			};

			glm::vec4 rayDir {(uv.x - 0.5) * 2.0 * uvSinX, (uv.y - 0.5) * 2.0 * uvSinY, 1.0, 0.0 };
			rayDir = cam->transform->getTransform() * rayDir;
			rayDir /= std::sqrt(rayDir.x*rayDir.x + rayDir.y*rayDir.y + rayDir.z*rayDir.z);

			Ray ray {
				Vector(cam->transform->position.x, cam->transform->position.y, cam->transform->position.z),
				Vector(cam->transform->position.x + rayDir.x, cam->transform->position.y + rayDir.y, cam->transform->position.z + rayDir.z) * rayLength
			};
			_imageData[i + j * _viewportWidth] = _rayTracer->getRayColor(ray).toRGBA2B();
		}
	}

	// Update image data
	_image->setData(_imageData);
}


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

	// Ray tracer instance
	_rayTracer = std::make_unique<RayTracer>();
}

void RayPipeline::render(CommandBuffer &commandBuffer, scene::WdeSceneInstance &scene) {
	// Recreate raytracing image if needed
	if (!_image || _viewportWidth != _image->getWidth() || _viewportHeight != _image->getHeight()) {
		if (_viewportWidth == 0 && _viewportHeight == 0) {
			_viewportWidth = 10;
			_viewportHeight = 10;
		}

		WaterDropEngine::get().getRender().getInstance().waitForDevicesReady();
		_image = std::make_shared<CImage>(_viewportWidth, _viewportHeight, ImageFormat::RGBA);
		delete[] _imageData;
		_imageData = new uint32_t[_viewportWidth * _viewportHeight];
		renderRayTracing();
	}

	// Render ray tracing
	static int acc = 0;
	acc++;
	if (acc >= 20) {
		acc = 0;
		renderRayTracing();
	}

	// Render pass (only gui)
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
	delete[] _imageData;
	_image.reset();
}

void RayPipeline::onNotify(const core::Event& event) {
	#ifdef WDE_GUI_ENABLED
		if (event.channel == LogChannel::GUI && event.name == "DrawGUI") {
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("RenderZone");
			_viewportWidth = static_cast<int>(ImGui::GetContentRegionAvail().x);
			_viewportHeight = static_cast<int>(ImGui::GetContentRegionAvail().y);
			if (_image)
				ImGui::Image(_image->getDescriptorSet(), { (float)_image->getWidth(), (float)_image->getHeight() });

			ImGui::End();
			ImGui::PopStyleVar();
		}
	#endif
}


#pragma clang diagnostic pop