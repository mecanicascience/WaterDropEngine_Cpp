#include "CoreAppScene.hpp"

void CoreAppScene::initialize() {
	// Add Camera
	auto& camera = createGameObject("Camera");
	camera.addModule<TransformControllerModule>();
	camera.addModule<CameraModule>();


	// Room
	/*auto& room = createGameObject("Room");
	room.addModule<ModelModule>(std::make_shared<ModelLoader>("sponza.obj"), std::make_shared<ColorMaterial>(RenderStage {0, 0}));*/

	// Materials
	// ColorMaterial - ColorMaterialOutline - TextureMaterial

	// Cube
	/*auto& cube = createGameObject("Cube");
	cube.addModule<ModelModule>(std::make_shared<ModelCube>(), std::make_shared<ColorMaterial>(RenderStage {0, 0}));
	auto& cubeTransform = cube.getModule<TransformModule>();
	cubeTransform.position = {0.0f, 0.0f, 2.5f};
	cubeTransform.scale /= 2.0f;*/

	// Flat Vase object
	/*auto& flatVase = createGameObject("Vase Flat");
	flatVase.addModule<ModelModule>(std::make_shared<ModelLoader>("flat_vase.obj"), std::make_shared<ColorMaterial>(RenderStage {0, 0}));
	auto& vaseTransform = flatVase.getModule<TransformModule>();
	vaseTransform.position = {-0.5f, 0.0f, 2.5f};
	vaseTransform.scale = {3.0f, 1.5f, 3.0f};
	vaseTransform.rotation = {glm::pi<float>(), 0.0f, 0.0f};

	// Smooth vase object
	auto& smoothVase = createGameObject("Vase Smooth");
	smoothVase.addModule<ModelModule>(std::make_shared<ModelLoader>("smooth_vase.obj"), std::make_shared<ColorMaterial>(RenderStage {0, 0}));
	auto& smoothVaseTransform = smoothVase.getModule<TransformModule>();
	smoothVaseTransform.position = {0.5f, 0.0f, 2.5f};
	smoothVaseTransform.scale = {3.0f, 1.5f, 3.0f};
	smoothVaseTransform.rotation = {glm::pi<float>(), 0.0f, 0.0f};*/

	// Viking room test game object
	/*auto& testGO = createGameObject("Viking Room");
	testGO.addModule<ModelModule>(std::make_shared<ModelLoader>("viking_room.obj"),
	                              std::make_shared<TextureMaterial>(RenderStage {0, 0}, "viking_room.png"));
	testGO.getModule<TransformModule>().rotation = {3*glm::half_pi<float>(), 0.0f, 0.0f};
	testGO.getModule<TransformModule>().scale /= 3.0f;*/


	// Minecraft scene test game object
	auto& mcScene = createGameObject("Minecraft Scene");
	mcScene.addModule<ModelModule>(std::make_shared<ModelLoader>("lost_empire.obj"),
								   std::make_shared<TextureMaterial>(RenderStage {0, 0}, "lost_empire.png", VK_FILTER_NEAREST));
}
