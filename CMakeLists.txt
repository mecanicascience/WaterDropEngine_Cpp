# Main Config
cmake_minimum_required(VERSION 3.19)
project(WaterDropEngine VERSION 0.1)
set(CMAKE_CXX_STANDARD 20)

# Added Requirements
find_package(Vulkan REQUIRED FATAL_ERROR)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)

# Setup executable and librairies refs
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)
