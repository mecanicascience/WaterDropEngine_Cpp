#pragma once

#include <WaterDropEngine/includes.hpp>

#include "01_DrawSimpleTriangle/DrawSimpleTriangleRenderer.hpp"
#include "02_DrawCube/DrawCubeRenderer.hpp"
#include "02_DrawCube/DrawCubeScene.hpp"
#include "03_LoadShapes/LoadShapesRenderer.hpp"
#include "03_LoadShapes/LoadShapesScene.hpp"
#include "04_DrawGUI/DrawGUIRenderer.hpp"
#include "04_DrawGUI/DrawGUIScene.hpp"

using namespace wde;
class ExamplesApp {
	public:
		/** Create a new application */
		ExamplesApp() = default;

		/** Starts the application */
		void start();
};

