#include "WdeSceneInstance.hpp"

namespace wde::scene {
	void WdeSceneInstance::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene instance." << logger::endl;

		// Update scene
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Updating scene instance." << logger::endl;
		update();
	}
}
