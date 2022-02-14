#include "WdeScene.hpp"

namespace wde::scene {
	WdeScene::WdeScene(std::shared_ptr<core::Subject> moduleSubject) : Module(std::move(moduleSubject)) {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Initializing Scene Engine ==" << logger::endl;

		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Initialization Done ==" << logger::endl;
	}

	void WdeScene::tick() {
		WDE_PROFILE_FUNCTION();
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "Ticking for scene engine." << logger::endl;
	}

	void WdeScene::cleanUp() {
		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Scene Engine ==" << logger::endl;

		logger::log(LogLevel::DEBUG, LogChannel::SCENE) << "== Cleaning Up Done ==" << logger::endl;
	}


	void WdeScene::onNotify(core::Event event) { }
}
