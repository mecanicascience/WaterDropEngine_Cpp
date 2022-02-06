# Main Config
cmake_minimum_required(VERSION 3.16)
project(WaterDropEngine VERSION 1.2.0)


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


# == CREATE APP USER APPLICATION ==
# Add client
add_executable(${PROJECT_NAME} app/main.cpp src/WaterDropEngine/WaterDropEngine.hpp src/WaterDropEngine/WdeCore/Core/Core.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.hpp src/WaterDropEngine/WdeCommon/WdeLogger/Logger.hpp src/WaterDropEngine/WdeCore/Structure/Subject.hpp src/WaterDropEngine/WdeRender/WdeRender.cpp src/WaterDropEngine/WdeRender/WdeRender.hpp src/WaterDropEngine/WdeGUI/WdeGUI.cpp src/WaterDropEngine/WdeGUI/WdeGUI.hpp src/WaterDropEngine/WdeCore/Structure/Observer.hpp src/wde.hpp src/WaterDropEngine/WdeCore/Structure/Event.hpp src/WaterDropEngine/WdeCore/Core/Module.hpp src/WaterDropEngine/WdeCommon/WdeException/WdeException.hpp src/WaterDropEngine/WdeCommon/WdeLogger/LoggerHandler.cpp src/WaterDropEngine/WdeCommon/WdeLogger/Instrumentation.hpp src/WaterDropEngine/WdeCommon/WdeUtils/NonCopyable.hpp src/WaterDropEngine/WdeCommon/WdeUtils/Config.hpp)

# Include libraries
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)
