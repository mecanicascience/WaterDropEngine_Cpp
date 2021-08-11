# Main Config
cmake_minimum_required(VERSION 3.17)
project(WaterDropEngine VERSION 0.1.0)

# Bin directory #
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/logs/)
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/res/)
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/res/shaders/)
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/res/models/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Require CXX
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)
add_subdirectory(lib/tinyobjloader)

# ImGui
add_library(imgui STATIC lib/imgui/imgui.cpp lib/imgui/imgui.h lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_internal.h lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imstb_rectpack.h lib/imgui/imstb_textedit.h lib/imgui/imstb_truetype.h lib/imgui/backends/imgui_impl_glfw.cpp lib/imgui/backends/imgui_impl_glfw.h lib/imgui/backends/imgui_impl_vulkan.cpp lib/imgui/backends/imgui_impl_vulkan.h)
target_link_libraries(imgui glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)

# Stb
add_library(stb STATIC lib/stb/stb_image.h)
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(stb)

# Library
add_library(${PROJECT_NAME} SHARED src/wde.hpp src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WaterDropEngine.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.cpp src/WaterDropEngine/WdeCommon/WdeError/WdeStatus.hpp src/WaterDropEngine/WdeCommon/WdeError/WdeException.hpp src/WaterDropEngine/WdeCore/WdeInstance.cpp src/WaterDropEngine/WdeCore/WdeInstance.hpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.cpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Constants.hpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.cpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.hpp src/WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDeviceHelper.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDeviceHelper.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/SwapChain.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/SwapChain.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandBuffer.cpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandBuffer.hpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandPool.cpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandPool.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderPassAttachment.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassType.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.hpp src/WaterDropEngine/WdeCommon/WdeMaths/Color.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPassVulkan.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPassVulkan.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassDescription.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/managers/SubrenderersManager.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderStage.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/managers/TypeInfo.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp src/WaterDropEngine/WdeRenderingEngine/buffers/Framebuffers.cpp src/WaterDropEngine/WdeRenderingEngine/buffers/Framebuffers.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/Pipeline.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.cpp src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/ShaderUtils.cpp src/WaterDropEngine/WdeRenderingEngine/pipelines/ShaderUtils.hpp src/WaterDropEngine/WdeScene/objects/Model.cpp src/WaterDropEngine/WdeScene/objects/Model.hpp src/WaterDropEngine/WdeRenderingEngine/buffers/BufferUtils.hpp src/WaterDropEngine/WdeRenderingEngine/images/ImageDepth.hpp src/WaterDropEngine/WdeRenderingEngine/images/Image.cpp src/WaterDropEngine/WdeRenderingEngine/images/Image.hpp src/WaterDropEngine/WdeRenderingEngine/utils/FPSUtils.hpp src/WaterDropEngine/WdeScene/objects/GameObject.hpp src/WaterDropEngine/WdeCore/WdeModule.hpp src/WaterDropEngine/WdeInputEngine/InputManager.cpp src/WaterDropEngine/WdeInputEngine/InputManager.hpp src/WaterDropEngine/WdeScene/objects/models/ModelCube.hpp src/WaterDropEngine/WdeScene/objects/models/ModelLoader.hpp src/WaterDropEngine/WdeGUI/WdeGUI.cpp src/WaterDropEngine/WdeGUI/WdeGUI.hpp src/WaterDropEngine/WdeRenderingEngine/images/Image2D.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/GUISubrenderer.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/GUISubrenderer.hpp src/WaterDropEngine/WdeScene/WdeSceneManager.cpp src/WaterDropEngine/WdeScene/WdeSceneManager.hpp src/WaterDropEngine/WdeScene/Scene.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/MeshSubrenderer.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/MeshSubrenderer.hpp src/WaterDropEngine/WdeScene/modules/Module.hpp src/WaterDropEngine/WdeGUI/GUITheme.hpp src/WaterDropEngine/WdeGUI/GUILogger.hpp src/WaterDropEngine/WdeGUI/GUIBar.hpp src/WaterDropEngine/WdeScene/modules/TransformModule.hpp src/WaterDropEngine/WdeGUI/GUIRenderer.hpp src/WaterDropEngine/WdeScene/modules/ModelModule.hpp src/WaterDropEngine/WdeScene/modules/CameraModule.hpp src/WaterDropEngine/WdeScene/modules/TransformControllerModule.hpp src/WaterDropEngine/WdeInputEngine/InputController.cpp src/WaterDropEngine/WdeInputEngine/InputController.hpp src/WaterDropEngine/WdeScene/materials/Material.hpp src/WaterDropEngine/WdeScene/materials/ColorMaterial.hpp src/WaterDropEngine/WdeScene/materials/ColorMaterialOutline.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/GizmoSubrenderer.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/subrenderers/GizmoSubrenderer.hpp src/WaterDropEngine/WdeScene/modules/ModelModule.cpp src/WaterDropEngine/WdeScene/gizmos/Gizmo.cpp src/WaterDropEngine/WdeScene/gizmos/Gizmo.hpp src/WaterDropEngine/WdeScene/gizmos/GizmoRenderer.hpp src/WaterDropEngine/WdeScene/gizmos/GizmoRenderer.cpp src/WaterDropEngine/WdeRenderingEngine/descriptors/DescriptorSet.hpp src/WaterDropEngine/WdeRenderingEngine/descriptors/Descriptor.hpp src/WaterDropEngine/WdeRenderingEngine/descriptors/DescriptorPool.hpp src/WaterDropEngine/WdeRenderingEngine/images/Texture2D.hpp src/WaterDropEngine/WdeRenderingEngine/images/Texture2D.cpp)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glm tinyobjloader imgui stb Vulkan::Vulkan -Wl,--allow-multiple-definition -static -static-libgcc -static-libstdc++)

# Add models
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/res/)

# Add shaders
file(GLOB SHADERS_LINK
    "res/shaders/*.frag.spv"
    "res/shaders/*.vert.spv"
    "res/shaders/*.frag"
    "res/shaders/*.vert"
)
file(COPY ${SHADERS_LINK} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/shaders)

# Add client
add_executable(${PROJECT_NAME}-App app/main.cpp app/CoreApp.hpp app/CoreApp.cpp app/render/CoreAppRenderer.hpp app/render/CoreAppRenderer.cpp app/scene/CoreAppScene.cpp app/scene/CoreAppScene.hpp)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/include)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyobjloader)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)
target_link_libraries(${PROJECT_NAME}-App PUBLIC Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME}-App PUBLIC WaterDropEngine)
