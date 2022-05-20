#pragma once

#include "GUIPanel.hpp"
#include "../../WdeRender/pipelines/PipelineCompute.hpp"
#include "../../WdeRender/images/Image2D.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"

namespace wde::gui {
	/**
	 * The GUI panel that draws the active chunks to the screen.
	 */
	class WorldPartitionPanel : public GUIPanel {
		public:
			~WorldPartitionPanel();

		private:
			struct WorldPartitionConfig {
				int imageSize;
				int chunkSize;
				int viewSize;
				int playerChunkIDX;
				int playerChunkIDY;
				int loadChunksCount;
				int activeChunksCount;
				int unloadChunksCount;
			};

			// Image
			std::unique_ptr<render::Image2D> _worldImage {};
			VkSampler _textureSampler = VK_NULL_HANDLE;
			ImTextureID _worldImageID = nullptr;

			// Compute pipeline
			std::unique_ptr<render::Buffer> _computeImConfig {};
			std::unique_ptr<render::Buffer> _loadingChBuffer {};
			std::unique_ptr<render::Buffer> _activeChBuffer {};
			std::unique_ptr<render::Buffer> _unloadChBuffer {};
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _imageSet {};
			std::unique_ptr<render::PipelineCompute> _computeImPipeline {};

			// Methods
			void render() override;
	};
}
