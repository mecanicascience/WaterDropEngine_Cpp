#pragma once

#include "../WdeCommon/WdeUtils/Config.hpp"
#include "../WdeCore/Core/Module.hpp"
#include "../WdeRender/descriptors/DescriptorPool.hpp"
#include "../WdeRender/commands/CommandBuffer.hpp"
#include "GUITheme.hpp"
#include "GUIBar.hpp"

namespace wde::gui {
	class WdeGUI : public core::Module {
		public:
			// Module commands
			explicit WdeGUI(std::shared_ptr<core::Subject> moduleSubject);

			/**
			 * Initialize the GUI (must be called by the pipeline)
			 * @param renderStage Render stage of the GUI (as pair pass ID, subpass ID)
			 */
			static void initialize(std::pair<int, int> renderStage);
			/**
			 * Render the subpass to the command buffer (must be called by the pipeline)
			 * @param commandBuffer
			 */
			static void render(render::CommandBuffer &commandBuffer);

			void tick() override;
			void cleanUp() override;

			// Notifications
			void onNotify(core::Event event) override {};

		private:
			// Core parameters
			/** True if the gui has been initialized */
			bool _initialized = false;

			/** Main root dockspace ID */
			const std::string DOCKSPACE_ROOT_ID = "Main Dockspace CoreWindow";
			/** ImGUI resources descriptor pool */
			std::shared_ptr<render::DescriptorPool> _imGUIdescriptorPool;
			/** ImGUI top bar */
			GUIBar _guiBar {};


			/**
			 * On a ImGui initialization error is thrown
			 * @param err
			 */
			static void OnGuiConfigError(VkResult err) {
				if (err == 0)
					return;
				logger::log(LogLevel::ERR, LogChannel::GUI) << "ImGui reported an error :" << err << logger::endl;
			}
	};
}
