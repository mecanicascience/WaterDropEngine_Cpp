# Main Config
cmake_minimum_required(VERSION 3.17)
project(WaterDropEngine VERSION 0.1.0)

# Bin directory #
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/logs/)
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/res/shaders/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

# Require CXX
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

# Library
add_library(${PROJECT_NAME} SHARED src/wde.hpp src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WaterDropEngine.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.cpp src/WaterDropEngine/WdeCommon/WdeError/WdeStatus.hpp src/WaterDropEngine/WdeCommon/WdeError/WdeException.hpp src/WaterDropEngine/WdeCore/WdeInstance.cpp src/WaterDropEngine/WdeCore/WdeInstance.hpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.cpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Constants.hpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.cpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.hpp src/WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Instrumentation.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDeviceHelper.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDeviceHelper.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/SwapChain.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/SwapChain.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp app/CoreAppRenderer.cpp app/CoreAppRenderer.hpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandBuffer.cpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandBuffer.hpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandPool.cpp src/WaterDropEngine/WdeRenderingEngine/commands/CommandPool.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderPassAttachment.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassType.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPass.hpp src/WaterDropEngine/WdeCommon/WdeMaths/Color.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPassVulkan.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/passes/RenderPassVulkan.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderSubpassDescription.hpp app/CoreAppSubrenderer.cpp app/CoreAppSubrenderer.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/managers/SubrenderersManager.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/descriptors/RenderStage.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/managers/TypeInfo.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/Subrenderer.hpp src/WaterDropEngine/WdeRenderingEngine/buffers/Framebuffers.cpp src/WaterDropEngine/WdeRenderingEngine/buffers/Framebuffers.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/Pipeline.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.cpp src/WaterDropEngine/WdeRenderingEngine/pipelines/PipelineGraphics.hpp src/WaterDropEngine/WdeRenderingEngine/pipelines/ShaderUtils.cpp src/WaterDropEngine/WdeRenderingEngine/pipelines/ShaderUtils.hpp src/WaterDropEngine/WdeRenderingEngine/structures/Model.cpp src/WaterDropEngine/WdeRenderingEngine/structures/Model.hpp src/WaterDropEngine/WdeRenderingEngine/buffers/BufferUtils.hpp src/WaterDropEngine/WdeRenderingEngine/images/ImageDepth.hpp src/WaterDropEngine/WdeRenderingEngine/images/Image.cpp src/WaterDropEngine/WdeRenderingEngine/images/Image.hpp)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan -Wl,--allow-multiple-definition -static -static-libgcc -static-libstdc++)

# Add shaders
configure_file(res/shaders/simpleShader.frag.spv "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/shaders/" COPYONLY)
configure_file(res/shaders/simpleShader.vert.spv "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/shaders/" COPYONLY)

# Add client
add_executable(${PROJECT_NAME}-App app/main.cpp app/CoreApp.hpp app/CoreApp.cpp)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/include)
target_link_libraries(${PROJECT_NAME}-App PUBLIC Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME}-App PUBLIC WaterDropEngine)
