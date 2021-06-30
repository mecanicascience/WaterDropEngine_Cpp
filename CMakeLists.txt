# Main Config
cmake_minimum_required(VERSION 3.19)
project(WaterDropEngine VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

# Setup executable and librairies refs
add_executable(${PROJECT_NAME}-Main src/main.cpp src/core/WdeWindow.hpp src/core/WdeWindow.cpp src/core/CoreApp.hpp src/core/CoreApp.cpp src/core/WdeInstanceDevice.cpp src/core/WdeInstanceDevice.hpp src/utils/Constants.cpp src/utils/Constants.hpp src/core/WdeSwapChain.cpp src/core/WdeSwapChain.hpp src/rendering/WdeDefaultGraphicPipeline.cpp src/rendering/WdeDefaultGraphicPipeline.hpp)

# Add shaders
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
FILE(COPY shaders/simpleShader.frag.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")
FILE(COPY shaders/simpleShader.vert.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")

# Setup target link libraries
target_link_libraries(${PROJECT_NAME}-Main glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)
