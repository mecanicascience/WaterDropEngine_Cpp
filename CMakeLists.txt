# Main Config
cmake_minimum_required(VERSION 3.19)
project(WaterDropEngine VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

# Setup executable and librairies refs
add_executable(${PROJECT_NAME}-Main src/main.cpp src/WaterDropEngine/core/WdeWindow.hpp src/WaterDropEngine/core/WdeWindow.cpp src/WaterDropEngine/core/CoreApp.hpp src/WaterDropEngine/core/CoreApp.cpp src/WaterDropEngine/core/WdeInstanceDevice.cpp src/WaterDropEngine/core/WdeInstanceDevice.hpp src/WaterDropEngine/utils/Constants.cpp src/WaterDropEngine/utils/Constants.hpp src/WaterDropEngine/rendering/WdeSwapChain.cpp src/WaterDropEngine/rendering/WdeSwapChain.hpp src/WaterDropEngine/rendering/WdeDefaultGraphicsPipeline.cpp src/WaterDropEngine/rendering/WdeDefaultGraphicsPipeline.hpp src/WaterDropEngine/utils/Utils.cpp src/WaterDropEngine/utils/Utils.hpp src/WaterDropEngine/rendering/WdePipeline.hpp src/WaterDropEngine/rendering/WdeRenderer.cpp src/WaterDropEngine/rendering/WdeRenderer.hpp src/WaterDropEngine/WaterDropEngine.hpp)

# Add shaders
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
FILE(COPY shaders/simpleShader.frag.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")
FILE(COPY shaders/simpleShader.vert.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")

# Setup target link libraries
target_link_libraries(${PROJECT_NAME}-Main glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)
