#include "WorldPartitionPanel.hpp"
#include "../../WaterDropEngine.hpp"

namespace wde::gui {
	void WorldPartitionPanel::render() {
		WDE_PROFILE_FUNCTION();
		// View 5 chunks around the player
		const int VIEW_SIZE = Config::CHUNK_SIZE * 20;
		const int IMAGE_SIZE = 800;

		ImGui::Begin("World Partition");
		auto scene = WaterDropEngine::get().getInstance().getScene();

		// Create compute pipeline and image
		static bool firstLaunch = true;
		if (firstLaunch) {
			firstLaunch = false;

			// Image generation
			{
				// Create rendering image
				_worldImage = std::make_unique<render::Image2D>(
						VK_FORMAT_R8G8B8A8_UNORM,
						VkExtent2D {IMAGE_SIZE, IMAGE_SIZE},
						VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
				_worldImage->createImage();

				// Create image view
				_worldImage->createImageView();

				// Create sampler
				resource::Texture2D::createTextureSampler(_textureSampler);

				// Generate texture ID
				resource::Texture2D::transitionImageLayout(*_worldImage, _worldImage->getLayout(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				_worldImageID = (ImTextureID) ImGui_ImplVulkan_AddTexture(_textureSampler, _worldImage->getView(), _worldImage->getLayout());
			}

			// Create compute pipeline
			{
				// Create compute buffer
				_computeImConfig = std::make_unique<render::Buffer>(
						sizeof(WorldPartitionConfig),
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

				// Create loading chunks buffer
				_loadingChBuffer = std::make_unique<render::Buffer>(
						sizeof(int) * 2 * Config::MAX_CHUNKS_COUNT,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

				// Create active chunks buffer
				_activeChBuffer = std::make_unique<render::Buffer>(
						sizeof(int) * 2 * Config::MAX_CHUNKS_COUNT,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

				// unloading active chunks buffer
				_unloadChBuffer = std::make_unique<render::Buffer>(
						sizeof(int) * 2 * Config::MAX_CHUNKS_COUNT,
						VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

				// Create pipeline
				auto path = WaterDropEngine::get().getInstance().getScene()->getPath();
				_computeImPipeline = std::make_unique<render::PipelineCompute>(path + "data/shaders/engine/gui/worldPartitionPanel.comp");
				resource::Texture2D::transitionImageLayout(*_worldImage, _worldImage->getLayout(), VK_IMAGE_LAYOUT_GENERAL);

				// Setup descriptors for image
				VkDescriptorImageInfo imageInfo {};
				imageInfo.imageLayout = _worldImage->getLayout();
				imageInfo.imageView = _worldImage->getView();
				imageInfo.sampler = _textureSampler;

				// Create descriptor
				render::DescriptorBuilder::begin()
					.bind_image(0, &imageInfo, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
					.bind_buffer(1, *_computeImConfig, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
					.bind_buffer(2, *_activeChBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
					.bind_buffer(3, *_unloadChBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
					.bind_buffer(4, *_loadingChBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build(_imageSet.first, _imageSet.second);

				// Initialize
				_computeImPipeline->addDescriptorSet(_imageSet.second);
				_computeImPipeline->initialize();
			}
		}

		render::CommandBuffer commandBuffer {true};

		// Update data buffers
		{
			auto cam = WaterDropEngine::get().getInstance().getScene()->getActiveCamera();
			double chunkSize = Config::CHUNK_SIZE;

			// Set loading chunks IDs
			void *computeLoadD = _loadingChBuffer->map();
			auto* data4 = (int*) computeLoadD;
			auto loadChunks = scene->getLoadingChunks();
			int k = 0;
			for (auto& c : loadChunks) {
				data4[k++] = int(c.x);
				data4[k++] = int(c.y);
			}
			_loadingChBuffer->unmap();

			// Set active chunks IDs
			void *computeActiveD = _activeChBuffer->map();
			auto* data = (int*) computeActiveD;
			auto& activeChunks = scene->getActiveChunks();
			int i = 0;
			for (auto& c : activeChunks) {
				data[i++] = int(c.first.x);
				data[i++] = int(c.first.y);
			}
			_activeChBuffer->unmap();

			// Set unloading chunks IDs
			void *computeUnloadD = _unloadChBuffer->map();
			auto* data3 = (int*) computeUnloadD;
			auto unloadChunks = scene->getUnloadingChunks();
			int j = 0;
			for (auto& c : unloadChunks) {
				data3[j++] = int(c.first.x);
				data3[j++] = int(c.first.y);
			}
			_unloadChBuffer->unmap();

			// Set config struct
			void *computeImData = _computeImConfig->map();
			auto* data2 = (WorldPartitionConfig*) computeImData;
			data2->imageSize = IMAGE_SIZE;
			data2->chunkSize = int(chunkSize);
			data2->playerChunkIDX = std::floor(cam->transform->position.x / chunkSize + 0.5);
			data2->playerChunkIDY = std::floor(cam->transform->position.z / chunkSize + 0.5);
			data2->viewSize = VIEW_SIZE;
			data2->loadChunksCount = int(loadChunks.size());
			data2->activeChunksCount = int(activeChunks.size());
			data2->unloadChunksCount = int(unloadChunks.size());
			_computeImConfig->unmap();
		}

		// Run compute shader
		{
			resource::Texture2D::transitionImageLayout(*_worldImage, _worldImage->getLayout(), VK_IMAGE_LAYOUT_GENERAL);

			// Bind descriptor
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
			                        _computeImPipeline->getLayout(), 0, 1, &_imageSet.first, 0, nullptr);

			// Bind pipeline
			_computeImPipeline->bind(commandBuffer);

			// Run compute shader
			vkCmdDispatch(commandBuffer, std::floor(double(IMAGE_SIZE) / 32.0), std::floor(double(IMAGE_SIZE) / 32.0), 1);
		}

		commandBuffer.end();
		commandBuffer.submit();
		commandBuffer.waitForQueueIdle();


		// Render image and display data
		{
			// Render world partition infos
			ImGui::Text("Chunk size : %i x %i.", Config::CHUNK_SIZE, Config::CHUNK_SIZE);
			ImGui::Text("Loading chunk count : %llu.",scene->getLoadingChunks().size());
			ImGui::Text("Active chunk count : %llu.", scene->getActiveChunks().size());
			ImGui::Text("Unloading chunk count : %llu.", scene->getUnloadingChunks().size());

			// Render image
			ImGui::Dummy(ImVec2(8.0f, 0.0f));
			resource::Texture2D::transitionImageLayout(*_worldImage, _worldImage->getLayout(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			ImGui::Image(_worldImageID, ImVec2(400.0f, 400.0f));
		}

		ImGui::End();
	}

	WorldPartitionPanel::~WorldPartitionPanel() {
		WDE_PROFILE_FUNCTION();

		// Destroy texture sampler
		vkDestroySampler(WaterDropEngine::get().getRender().getInstance().getDevice().getDevice(), _textureSampler, nullptr);

		// Destroy image
		_worldImage.reset();
	}
}
