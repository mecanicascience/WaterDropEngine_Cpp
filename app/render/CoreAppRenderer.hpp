#pragma once

#include <WaterDropEngine/includes.hpp>

using namespace wde;
using namespace wde::renderEngine;
using namespace wde::scene;

class CoreAppRenderer : public Renderer {
	public:
		CoreAppRenderer() = default;
		~CoreAppRenderer() override = default;

		/** Initialize the renderer */
		void initialize() override;
		/** Starts the renderer */
		void start() override;
};
