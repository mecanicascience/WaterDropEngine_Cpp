#pragma once

#include "../../../wde.hpp"
#include "../GameObject.hpp"
#include "../modules/MeshRendererModule.hpp"
#include "../modules/CameraModule.hpp"
#include "../../WdeRender/buffers/Buffer.hpp"
#include "../../WdeScene/GameObject.hpp"
#include "TerrainTile.hpp"

#include <utility>

namespace wde::scene {
	class WdeSceneInstance;

	/**
	 * Class that represents a scene terrain chunk.
	 */
	class Chunk {
		public:
			/** Camera data */
			struct GPUCameraData {
				glm::mat4 view {1.0f};
				glm::mat4 proj {1.0f};
			};

			// Constructors
			explicit Chunk(WdeSceneInstance* sceneInstance, glm::ivec2 pos);
			/** Saves the chunk data to the associated chunk file */
			void save();
			~Chunk();

			// Common methods
			void tick();
			void updateGOBuffers();
			void bind(render::CommandBuffer &commandBuffer, resource::Material *material) const;
			void drawGUI();


			// Getters
			std::vector<std::shared_ptr<GameObject>>& getGameObjects() { return _gameObjects; }
			std::vector<std::shared_ptr<GameObject>>& getStaticGameObjects()  { return _gameObjectsStatic; }
			std::vector<std::shared_ptr<GameObject>>& getDynamicGameObjects() { return _gameObjectsDynamic; }
			std::pair<VkDescriptorSet, VkDescriptorSetLayout>& getGlobalSet() { return _globalSet; }
			std::pair<VkDescriptorSet, VkDescriptorSetLayout>& getCullingSet() { return _cullingSet; }
			std::unique_ptr<render::Buffer>& getCullingSceneBuffer() { return _cullingSceneBuffer; }
			static bool isCullingEnabled() { return _cullingEnabled; }
			static bool* isCullingEnabledPtr() { return &_cullingEnabled; }
			static bool showGOBoundingBoxes() { return _showGOBoundingBox; }
			static bool* showGOBoundingBoxesPtr() { return &_showGOBoundingBox; }


			// Game Objects Manager
			/**
			 * Add a pre-existing game object to the list
			 * @param go
			 */
			void addGameObject(const std::shared_ptr<GameObject>& go) {
				// Add to list
				_gameObjects.push_back(go);
				if (go->isStatic())
					_gameObjectsStatic.push_back(go);
				else
					_gameObjectsDynamic.push_back(go);
			}
			/**
			 * Create a new GameObject
			 * @param name Name of the GameObject
			 * @param isStatic True if the game object is a static one (default false)
			 */
			std::shared_ptr<GameObject> createGameObject(const std::string& name, bool isStatic = false) {
				auto goPtr = std::make_shared<GameObject>(Chunk::_gameObjectsIDCurr++, name, isStatic);
				_gameObjects.push_back(goPtr);
				if (isStatic)
					_gameObjectsStatic.push_back(goPtr);
				else
					_gameObjectsDynamic.push_back(goPtr);
				return goPtr;
			}

			/**
			 * Remove a given GameObject
			 * @param go
			 */
			void removeGameObject(GameObject* go) {
				// Remove GameObject
				_gameObjectsToDelete.push_back(go);
			}
			/** Clear the game objects list */
			void clearGameObjects() {
				_gameObjects.clear();
				_gameObjectsStatic.clear();
				_gameObjectsDynamic.clear();
			}


		private:
			// Chunk data
			WdeSceneInstance* _sceneInstance;
			glm::ivec2 _pos;

			// Chunk visualisation
			static bool _cullingEnabled;
			static bool _showGOBoundingBox;

			// Chunk game objects handling
			/** List of all scene game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjects {};
			/** List of scene static game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsStatic {};
			/** List of scene dynamic game objects */
			std::vector<std::shared_ptr<GameObject>> _gameObjectsDynamic {};
			/** List of all scene game objects to delete on next tick */
			std::vector<GameObject*> _gameObjectsToDelete {};
			/** Chunk terrain instance */
			std::unique_ptr<TerrainTile> _terrainTile {};

			// Chunk game objects data
			/** Last create game object ID */
			uint32_t _gameObjectsIDCurr = 0;

			// Passes common descriptor sets
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _globalSet;
			std::unique_ptr<render::Buffer> _cameraData;
			std::unique_ptr<render::Buffer> _objectsData;

			// Culling
			std::unique_ptr<render::Buffer> _cullingSceneBuffer;
			std::pair<VkDescriptorSet, VkDescriptorSetLayout> _cullingSet;
			
			// Helper functions
			/**
			 * Draw the GUI for a given game object
			 * @param go GameObject
			 * @param selected True if the GameObject is selected
			 */
			void drawGUIForGo(GameObject* go, GameObject*& selected) const;
	};
}
