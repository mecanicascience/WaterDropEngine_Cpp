#include "examples/01-Triangle/EngineInstanceExample01.hpp"
#include "examples/02-3D_Cube/EngineInstanceExample02.hpp"
#include "examples/03-Draw_Indirect/EngineInstanceExample03.hpp"
#include "examples/04-Indirect_Culling/EngineInstanceExample04.hpp"
#include "examples/05-Terrain/EngineInstanceExample05.hpp"
#include "rayTracer/RayEngineInstance.hpp"

int main() {
	// === EXAMPLES ===
	{
		// 01 - Triangle
		//examples::EngineInstanceExample01 instance01 {};
		//instance01.startInstance();

		// 02 - 3D Cube
		//examples::EngineInstanceExample02 instance02 {};
		//instance02.startInstance();

		// 03 - Draw Instanced
		//examples::EngineInstanceExample03 instance03 {};
		//instance03.startInstance();

		// 04 - Indirect Culling
		//examples::EngineInstanceExample04 instance04 {};
		//instance04.startInstance();

		// 05 - Terrain
		//examples::EngineInstanceExample05 instance05 {};
		//instance05.startInstance();
	}

	// Ray Tracing
	rtx::RayEngineInstance instance {};
	instance.startInstance();

	return 0;
}
