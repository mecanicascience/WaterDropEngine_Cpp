#pragma once

#include "../../../wde.hpp"
#include "passes/RenderPass.hpp"
#include "managers/SubrenderersManager.hpp"
#include "descriptors/RenderStage.hpp"

namespace wde::renderEngine {
	/**
	 * Class used to manage RenderPass objects to create a list of render pass.
	 */
	class Renderer : NonCopyable {
		public:
			// Core functions
			/**
			 * Create a new renderer.
			 * Use this method to create the RenderPass objects to create a list of render pass.
			 */
			Renderer() = default;
			virtual ~Renderer() = default;

			void cleanUp() {
				// Clean up render passes
				for (auto& pass : _renderPasses) {
					pass->cleanUp();
				}
			};

			/** Initialize the renderer */
			virtual void initialize() = 0;
			/** Starts the renderer */
			virtual void start() = 0;


			// Setters
			/**
			 * Add a render pass to the renderer list
			 * @param renderPass
			 */
			void addRenderPass(std::unique_ptr<RenderPass> &&renderPass) {
				_renderPasses.emplace_back(std::move(renderPass));
			}
			/**
			 * Adds a Sub-render
			 * @tparam T The Sub-render type
			 * @param renderStage The Sub-render stage { render pass ID, sub-passID }
			 * @tparam Args The constructor arg types
			 * @param args The constructor arguments
			 */
			template<typename T, typename... Args>
			T *addSubrenderer(const RenderStage &renderStage, Args &&...args) {
				return _subrenderersManager.add<T>(renderStage, std::make_unique<T>(renderStage, std::forward<Args>(args)...));
			}

			// Getters
			std::vector<std::unique_ptr<RenderPass>>& getRenderPasses() { return _renderPasses; }
			SubrenderersManager& getSubRenderersManager() { return _subrenderersManager; }
			/** @return the render pass given the pass index */
			RenderPass *getRenderPass(uint32_t index) const {
				if (_renderPasses.empty() || _renderPasses.size() < index)
					return nullptr;
				return _renderPasses.at(index).get();
			}


		private:
			/** The list of every render passes */
			std::vector<std::unique_ptr<RenderPass>> _renderPasses;
			/** The sub-renderers manager class */
			SubrenderersManager _subrenderersManager {};
	};
}
