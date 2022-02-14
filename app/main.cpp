#include "examples/01-Triangle/EngineInstanceExample01.hpp"
#include "examples/02-3D_Cube/EngineInstanceExample02.hpp"

int main() {
	// === EXAMPLES ===
	{
		// 01 - Triangle
		//examples::EngineInstanceExample01 instance01 {};
		//instance01.startInstance();

		// 02 - 3D Cube
		examples::EngineInstanceExample02 instance02 {};
		instance02.startInstance();
	}

	return 0;
}
