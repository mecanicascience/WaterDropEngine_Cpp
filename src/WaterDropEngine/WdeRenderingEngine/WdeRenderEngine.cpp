#include "WdeRenderEngine.hpp"
#include "../WdeCommon/WdeLogger/Logger.hpp"

namespace wde::renderEngine {
	WdeStatus WdeRenderEngine::initialize() {
		Logger::debug("== Initializing Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		// Initialize here ...
		Logger::debug("== Initialization Done ==", LoggerChannel::RENDERING_ENGINE);

		return WdeStatus::WDE_SUCCESS;
	}


	WdeStatus WdeRenderEngine::tick() {
		return WdeStatus::WDE_SUCCESS;
	}


	WdeStatus WdeRenderEngine::cleanUp() {
		Logger::debug("== Cleaning up Rendering Engine ==", LoggerChannel::RENDERING_ENGINE);
		// Clean up here ...
		Logger::debug("== Cleaning up Done ==", LoggerChannel::RENDERING_ENGINE);

		return WdeStatus::WDE_SUCCESS;
	}


	bool WdeRenderEngine::shouldEnd() {
		return true;
	}
}
