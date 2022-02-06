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

# ImGui
add_library(imgui STATIC lib/imgui/imgui.h lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_internal.h lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/imstb_rectpack.h lib/imgui/imstb_textedit.h lib/imgui/imstb_truetype.h lib/imgui/backends/imgui_impl_glfw.cpp lib/imgui/backends/imgui_impl_glfw.h lib/imgui/backends/imgui_impl_vulkan.cpp lib/imgui/backends/imgui_impl_vulkan.h)
target_link_libraries(imgui glfw ${GLFW_LIBRARIES} glm Vulkan::Vulkan)

# == CREATE APP USER APPLICATION ==
# Add client
add_executable(${PROJECT_NAME} app/main.cpp src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WdeCore/Core/Core.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCore/Structure/Subject.hpp src/WaterDropEngine/WdeRender/WdeRender.cpp src/WaterDropEngine/WdeRender/WdeRender.hpp src/WaterDropEngine/WdeGUI/WdeGUI.cpp src/WaterDropEngine/WdeGUI/WdeGUI.hpp src/WaterDropEngine/WdeCore/Structure/Observer.hpp src/wde.hpp src/WaterDropEngine/WdeCore/Structure/Event.hpp src/WaterDropEngine/WdeCore/Core/Module.hpp src/WaterDropEngine/WdeCommon/WdeException/WdeException.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp src/WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Config.hpp src/WaterDropEngine/WdeGUI/GUITheme.hpp src/WaterDropEngine/WdeGUI/GUIRenderer.hpp)

# Include libraries
target_link_libraries(${PROJECT_NAME} PUBLIC imgui glfw glm Vulkan::Vulkan)

# Add res folder
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res/ ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/res/)

