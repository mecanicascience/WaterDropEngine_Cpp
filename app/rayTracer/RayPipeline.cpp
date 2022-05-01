#include <random>
#include "RayPipeline.hpp"
#include "../../src/WaterDropEngine/WaterDropEngine.hpp"
#include "tracer/Ray.hpp"


bool hit_sphere(const glm::dvec3& center, double radius, const Ray& r) {
	vec3 oc = r.getFrom() - center;
	auto a = dot(r.getDir(), r.getDir());
	auto b = 2.0 * (oc.x*r.getDir().x + oc.y*r.getDir().y + oc.z*r.getDir().z);
	auto c = dot(oc, oc) - radius*radius;
	auto discriminant = b*b - 4*a*c;
	return (discriminant > 0);
}

Color rayColor(const Ray& r) {
	if (hit_sphere(glm::dvec3 {-4.0, 0.0, 0.0}, 1.0, r))
		return Color(1.0, 1.0, 1.0);
	return Color(0.0, 0.0, 0.0);
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "DanglingPointer"
void RayPipeline::renderRayTracing() {
	if (_imageData == nullptr)
		return;

	// Get camera
	auto cam = WaterDropEngine::get().getInstance().getScene()->getActiveCamera();
	if (cam == nullptr)
		return;
	auto camMod = cam->getModule<scene::CameraModule>();
	auto aspectRatio = camMod->getAspect();

	// Do ray tracing
	for (int j = _viewportHeight - 1; j >= 0; j--) {
		for (int i = 0; i < _viewportWidth; i++) {
			// Pixel UV coordinate (from (0, 0) at bottom left to (1, 1) at top right)
			glm::vec2 uv {float(i) / static_cast<float>(_viewportWidth - 1), float(_viewportHeight - 1 - j) / static_cast<float>(_viewportHeight - 1)};

			auto thetaDiv2 = std::tan(glm::radians(camMod->getFov()) / 2.0f);
			auto wNear = camMod->getAspect() * camMod->getNear() * thetaDiv2;
			auto bottomNearLeft  = cam->transform->getTransform() * glm::vec4 {-wNear, -camMod->getNear() * thetaDiv2, camMod->getFar(), 1};
			glm::vec4 rayDir = bottomNearLeft + glm::vec4 {uv.x * static_cast<float>(_viewportHeight) * aspectRatio, uv.y * static_cast<float>(_viewportHeight), 0.0, 0.0};
			rayDir /= std::sqrt(rayDir.x*rayDir.x + rayDir.y*rayDir.y + rayDir.z*rayDir.z);

			Ray ray {cam->transform->position, glm::vec3(rayDir.x, rayDir.y, rayDir.z) * 100.0f};
			_imageData[i + j * _viewportWidth] = rayColor(ray).toRGBA2B();

			/*// Compute ray direction (suppose camera pointing to x-axis)
			glm::vec4 rayDir {
				camMod->getNear(),
				(uv.y - 0.5) * 2.0 * _viewportHeight,
				(uv.x - 0.5) * 2.0 * _viewportHeight * aspectRatio,
				0.0
			};
			//rayDir = rayDir * camMod->getView();
			rayDir /= std::sqrt(rayDir.x*rayDir.x + rayDir.y*rayDir.y + rayDir.z*rayDir.z);

			// Create ray from camera
			Ray ray {cam->transform->position, glm::vec3(rayDir.x, rayDir.y, rayDir.z) * 1000.0f};
			_imageData[i + j * _viewportWidth] = rayColor(ray).toRGBA2B();*/

			// Compute color
			//Color color {std::abs(rayDir.x * 255) / static_cast<float>(_viewportHeight) * aspectRatio, std::abs(rayDir.y * 255) / static_cast<float>(_viewportHeight), 0, 1};
			//_imageData[i + j * _viewportWidth] = color.toRGBA2B();
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
	if (acc >= 100) {
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
			_viewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
			_viewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);
			if (_image)
				ImGui::Image(_image->getDescriptorSet(), { (float)_image->getWidth(), (float)_image->getHeight() });

			ImGui::End();
			ImGui::PopStyleVar();
		}
	#endif
}


#pragma clang diagnostic pop