#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "../../WdeRenderingEngine/renderer/descriptors/RenderStage.hpp"
#include "../../WdeRenderingEngine/commands/CommandBuffer.hpp"
#include "../gizmos/Gizmo.hpp"

using namespace wde::renderEngine;

namespace wde::scene {
	class GameObject;

	/**
	 * A class that represents a GameObject module type
	 */
	class Module {
		public:
			// Constructors
			/**
			 * Creates a new Module type
			 * @param moduleName The name of the module
			 */
			explicit Module(GameObject &gameObject, std::string moduleName) : _gameObject(gameObject), _moduleName(std::move(moduleName)) {}
			virtual ~Module() = default;


			// Core functions
			/** Initialize the module */
			virtual void initialize() {};
			/**
			 * Updates the module
			 * @param deltaTime Time since last update (in seconds)
			 */
			virtual void update(float deltaTime) {};
			/**
			 * Performs render operations given the command buffer during a render stage
			 * @param commandBuffer The command buffer to submit render operations to
			 * @param stage The rendering stage
			 */
			virtual void render(CommandBuffer& commandBuffer, RenderStage stage) {};
			/**
			 * Render Game object gizmo
			 * @param gizmo The gizmo instance
			 */
			virtual void renderGizmo(Gizmo &gizmo) {}
			/** Render the module GUI */
			virtual void renderGUI() {};
			/** Serialize the module */
			virtual json serialize() { return json(""); };
			/** Cleaning up the module */
			virtual void cleanUp() {}


			/** Serialize the module */
			json serializeModule() {
				json initialJSON;
				// Module global data
				initialJSON["name"] = _moduleName;

				// Module specific data
				json serializeData = serialize();
				if (serializeData != json(""))
					initialJSON["data"] = serializeData;

				// Return json
				return initialJSON;
			}


			// Getters and setters
			std::string& getModuleName() { return _moduleName; }


		protected:
			/** The unique identifier of the type of the module */
			std::string _moduleName;
			/** A reference to the component game object */
			GameObject &_gameObject;
	};
}

