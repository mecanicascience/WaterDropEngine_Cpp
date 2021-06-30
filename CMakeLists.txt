# Main Config
cmake_minimum_required(VERSION 3.19)
project(WaterDropEngine VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

# Setup executable and librairies refs
add_executable(${PROJECT_NAME}-Main src/main.cpp src/engine/WdeWindow.hpp src/engine/WdeWindow.cpp src/FirstApp.hpp src/FirstApp.cpp src/engine/WdePipeline.cpp src/engine/WdePipeline.hpp src/engine/WdeDevice.cpp src/engine/WdeDevice.hpp src/engine/WdeInstanceDevice.cpp src/engine/WdeInstanceDevice.hpp src/Constants.cpp src/Constants.hpp)

# Add shaders
FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
FILE(COPY shaders/simpleShader.frag.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")
FILE(COPY shaders/simpleShader.vert.spv DESTINATION "${CMAKE_BINARY_DIR}/shaders/")

# Setup target link libraries
target_link_libraries(${PROJECT_NAME}-Main glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)
