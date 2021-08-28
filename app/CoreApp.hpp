#pragma once

#include <WaterDropEngine/includes.hpp>

#include "examples/01_DrawSimpleTriangle/DrawSimpleTriangleRenderer.hpp"
#include "examples/02_DrawCube/DrawCubeRenderer.hpp"
#include "examples/02_DrawCube/DrawCubeScene.hpp"
#include "examples/03_LoadShapes/LoadShapesRenderer.hpp"
#include "examples/03_LoadShapes/LoadShapesScene.hpp"
#include "examples/04_DrawGUI/DrawGUIRenderer.hpp"
#include "examples/04_DrawGUI/DrawGUIScene.hpp"

using namespace wde;
class CoreApp {
	public:
		/** Create a new application */
		CoreApp() = default;

		/** Starts the application */
		void start();
};
