# Main Config
cmake_minimum_required(VERSION 3.17)
project(WaterDropEngine VERSION 0.1.0)

# Bin directory #
file(MAKE_DIRECTORY bin/${CMAKE_BUILD_TYPE}/)
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
add_library(${PROJECT_NAME} SHARED src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WaterDropEngine.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.cpp src/WaterDropEngine/WdeCommon/WdeError/WdeStatus.hpp src/WaterDropEngine/WdeCommon/WdeError/WdeException.hpp src/WaterDropEngine/WdeCore/WdeInstance.cpp src/WaterDropEngine/WdeCore/WdeInstance.hpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.cpp src/WaterDropEngine/WdeRenderingEngine/WdeRenderEngine.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreWindow.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreInstance.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreDevice.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/SwapChain.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/SwapChain.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/Renderer.hpp src/WaterDropEngine/WdeRenderingEngine/renderer/GraphicsPipeline.cpp src/WaterDropEngine/WdeRenderingEngine/renderer/GraphicsPipeline.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Constants.cpp src/WaterDropEngine/WdeCommon/WdeUtils/Constants.hpp src/WaterDropEngine/WdeRenderingEngine/core/CoreUtils.cpp src/WaterDropEngine/WdeRenderingEngine/core/CoreUtils.hpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.cpp src/WaterDropEngine/WdeCommon/WdeFiles/WdeFileUtils.hpp src/WaterDropEngine/WdeRenderingEngine/renderObjects/Model.cpp src/WaterDropEngine/WdeRenderingEngine/renderObjects/Model.hpp src/WaterDropEngine/WdeRenderingEngine/utils/RenderingUtils.cpp src/WaterDropEngine/WdeRenderingEngine/utils/RenderingUtils.hpp src/WaterDropEngine/WdeRenderingEngine/renderObjects/GameObject.hpp)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan -Wl,--allow-multiple-definition -static -static-libgcc -static-libstdc++)

# Add shaders
configure_file(res/shaders/simpleShader.frag.spv "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/shaders/" COPYONLY)
configure_file(res/shaders/simpleShader.vert.spv "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/shaders/" COPYONLY)

# Add client
add_executable(${PROJECT_NAME}-App app/main.cpp app/CoreApp.hpp app/CoreApp.cpp app/renderEngine/CoreAppPipeline.cpp app/renderEngine/CoreAppPipeline.hpp app/renderEngine/CoreAppRenderer.cpp app/renderEngine/CoreAppRenderer.hpp)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME}-App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/include)
target_link_libraries(${PROJECT_NAME}-App PUBLIC Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME}-App PUBLIC WaterDropEngine)
