# Main Config
cmake_minimum_required(VERSION 3.16)
project(WaterDropEngine VERSION 2.0.0)


# == SETUP PROJECT CONFIGURATION ==
# Bin directory #
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/logs/)
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/profiler/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Require CXX
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# == ADD REQUIREMENTS ==
# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)
add_subdirectory(lib/tinyobjloader)
add_subdirectory(lib/portable-file-dialogs)

# ImGui
add_library(imgui STATIC lib/imgui/imgui.h lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_internal.h lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imstb_rectpack.h lib/imgui/imstb_textedit.h lib/imgui/imstb_truetype.h lib/imgui/backends/imgui_impl_glfw.cpp lib/imgui/backends/imgui_impl_glfw.h lib/imgui/backends/imgui_impl_vulkan.cpp lib/imgui/backends/imgui_impl_vulkan.h lib/imgui/imstb_rectpack.h lib/imgui/imstb_textedit.h lib/imgui/imstb_truetype.h)
target_link_libraries(imgui glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)

# PFD
add_library(pfd STATIC lib/portable-file-dialogs/portable-file-dialogs.h)
set_target_properties(pfd PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(pfd)

# Stb
add_library(stb STATIC lib/stb/stb_image.h)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(stb)



# == CREATE APP USER APPLICATION ==
# Add client
add_executable(${PROJECT_NAME} app/examples/01-Triangle/EngineInstanceExample01.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Config.hpp src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCore/Structure/Subject.hpp src/WaterDropEngine/WdeRender/WdeRender.cpp src/WaterDropEngine/WdeRender/WdeRender.hpp src/WaterDropEngine/WdeGUI/WdeGUI.cpp src/WaterDropEngine/WdeGUI/WdeGUI.hpp src/WaterDropEngine/WdeCore/Structure/Observer.hpp src/wde.hpp src/WaterDropEngine/WdeCore/Structure/Event.hpp src/WaterDropEngine/WdeCore/Core/Module.hpp src/WaterDropEngine/WdeCommon/WdeException/WdeException.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp src/WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp src/WaterDropEngine/WdeGUI/GUITheme.hpp src/WaterDropEngine/WdeGUI/GUIRenderer.hpp src/WaterDropEngine/WdeRender/core/CoreWindow.cpp src/WaterDropEngine/WdeRender/core/CoreWindow.hpp src/WaterDropEngine/WdeRender/core/CoreInstance.cpp src/WaterDropEngine/WdeRender/core/CoreInstance.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Config.cpp src/WaterDropEngine/WdeRender/core/CoreDevice.cpp src/WaterDropEngine/WdeRender/core/CoreDevice.hpp src/WaterDropEngine/WdeRender/render/Swapchain.cpp src/WaterDropEngine/WdeRender/render/Swapchain.hpp src/WaterDropEngine/WdeRender/commands/CommandPool.cpp src/WaterDropEngine/WdeRender/commands/CommandPool.hpp src/WaterDropEngine/WdeRender/commands/CommandBuffer.cpp src/WaterDropEngine/WdeRender/commands/CommandBuffer.hpp app/main.cpp src/WaterDropEngine/WdeCore/Core/WdeInstance.hpp src/WaterDropEngine/WdeCore/Core/WdeInstance.cpp src/WaterDropEngine/WdeCommon/WdeUtils/FPSUtils.hpp src/WaterDropEngine/WdeRender/render/RenderPass.cpp src/WaterDropEngine/WdeRender/render/RenderPass.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Color.hpp app/examples/01-Triangle/PipelineExample01.hpp src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.hpp src/WaterDropEngine/WdeRender/WdeRenderPipelineInstance.cpp src/WaterDropEngine/WdeRender/render/RenderAttachment.hpp src/WaterDropEngine/WdeRender/render/RenderPassStructure.hpp src/WaterDropEngine/WdeRender/images/Image.cpp src/WaterDropEngine/WdeRender/images/Image.hpp src/WaterDropEngine/WdeRender/images/ImageDepth.hpp src/WaterDropEngine/WdeRender/buffers/BufferUtils.hpp src/WaterDropEngine/WdeRender/buffers/FrameBuffers.cpp src/WaterDropEngine/WdeRender/buffers/FrameBuffers.hpp src/WaterDropEngine/WdeRender/images/Image2D.hpp src/WaterDropEngine/WdeRender/pipelines/Pipeline.hpp src/WaterDropEngine/WdeRender/pipelines/PipelineGraphics.cpp src/WaterDropEngine/WdeRender/pipelines/PipelineGraphics.hpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.cpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.hpp src/WaterDropEngine/WdeRender/pipelines/ShaderUtils.cpp src/WaterDropEngine/WdeRender/pipelines/ShaderUtils.hpp src/WaterDropEngine/WdeRender/buffers/Buffer.cpp src/WaterDropEngine/WdeRender/buffers/Buffer.hpp src/WaterDropEngine/WdeGUI/GUIBar.cpp src/WaterDropEngine/WdeGUI/GUIBar.hpp app/examples/02-3D_Cube/PipelineExample02.hpp app/examples/02-3D_Cube/EngineInstanceExample02.hpp src/WaterDropEngine/WdeScene/WdeScene.cpp src/WaterDropEngine/WdeScene/WdeScene.hpp src/WaterDropEngine/WdeScene/WdeSceneInstance.cpp src/WaterDropEngine/WdeScene/WdeSceneInstance.hpp app/examples/02-3D_Cube/SceneExample02.hpp src/WaterDropEngine/WdeScene/gameObjects/GameObject.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/Module.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/TransformModule.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/CameraModule.cpp src/WaterDropEngine/WdeScene/gameObjects/modules/MeshRendererModule.hpp src/WaterDropEngine/WdeScene/gameObjects/materials/Material.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/TransformModule.cpp src/WaterDropEngine/WdeScene/gameObjects/materials/ColorMaterial.hpp src/WaterDropEngine/WdeScene/gameObjects/meshes/Mesh.hpp src/WaterDropEngine/WdeScene/gameObjects/meshes/Vertex.hpp src/WaterDropEngine/WdeScene/gameObjects/meshes/Mesh.cpp src/WaterDropEngine/WdeRender/descriptors/DescriptorAllocator.cpp src/WaterDropEngine/WdeRender/descriptors/DescriptorAllocator.hpp src/WaterDropEngine/WdeRender/descriptors/DescriptorLayoutCache.cpp src/WaterDropEngine/WdeRender/descriptors/DescriptorLayoutCache.hpp src/WaterDropEngine/WdeRender/descriptors/DescriptorBuilder.cpp src/WaterDropEngine/WdeRender/descriptors/DescriptorBuilder.hpp src/WaterDropEngine/WdeRender/descriptors/DescriptorPool.cpp src/WaterDropEngine/WdeRender/descriptors/DescriptorPool.hpp src/WaterDropEngine/WdeScene/gameObjects/materials/Material.cpp app/examples/01-Triangle/SceneExample01.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/ControllerModule.hpp src/WaterDropEngine/WdeInput/InputController.cpp src/WaterDropEngine/WdeInput/InputController.hpp src/WaterDropEngine/WdeInput/InputManager.cpp src/WaterDropEngine/WdeInput/InputManager.hpp src/WaterDropEngine/WdeScene/gameObjects/meshes/MeshLoader.hpp app/examples/03-Draw_Indirect/EngineInstanceExample03.hpp app/examples/03-Draw_Indirect/PipelineExample03.hpp app/examples/03-Draw_Indirect/SceneExample03.hpp app/examples/04-Indirect_Culling/EngineInstanceExample04.hpp app/examples/04-Indirect_Culling/PipelineExample04.hpp app/examples/04-Indirect_Culling/SceneExample04.hpp src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.cpp src/WaterDropEngine/WdeRender/pipelines/PipelineCompute.hpp src/WaterDropEngine/WdeScene/gizmo/Gizmo.cpp src/WaterDropEngine/WdeScene/gizmo/Gizmo.hpp src/WaterDropEngine/WdeScene/gizmo/GizmoManager.cpp src/WaterDropEngine/WdeScene/gizmo/GizmoManager.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Color.cpp src/WaterDropEngine/WdeScene/gameObjects/materials/GizmoMaterial.hpp src/WaterDropEngine/WdeScene/gameObjects/materials/TextureMaterial.hpp src/WaterDropEngine/WdeRender/images/Texture2D.cpp src/WaterDropEngine/WdeRender/images/Texture2D.hpp app/examples/05-Terrain/EngineInstanceExample05.hpp app/examples/05-Terrain/PipelineExample05.hpp app/examples/05-Terrain/SceneExample05.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/TerrainModule.hpp src/WaterDropEngine/WdeScene/gameObjects/modules/TerrainModule.cpp app/examples/05-Terrain/ShadowMaterial.hpp src/WaterDropEngine/WdeRender/images/TextureCube.cpp src/WaterDropEngine/WdeRender/images/TextureCube.hpp src/WaterDropEngine/WdeScene/gameObjects/materials/SkyboxMaterial.hpp app/examples/04-Indirect_Culling/CullingInstance.hpp)

# Include libraries
target_link_libraries(${PROJECT_NAME} PUBLIC imgui glfw stb glm Vulkan::Vulkan tinyobjloader pfd)

# Add res folder
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/res/)

