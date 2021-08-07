#pragma once

#include <utility>

#include "../../../wde.hpp"

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
			 * Performs render operations given the command buffer
			 * @param commandBuffer The command buffer to submit render operations to
			 */
			virtual void render(CommandBuffer& commandBuffer) {};
			/** Render the module GUI */
			virtual void renderGUI() {};
			/** Cleaning up the module */
			virtual void cleanUp() {};


			// Getters and setters
			std::string& getModuleName() { return _moduleName; }


		protected:
			/** The unique identifier of the type of the module */
			std::string _moduleName;
			/** A reference to the component game object */
			GameObject &_gameObject;
	};
}

