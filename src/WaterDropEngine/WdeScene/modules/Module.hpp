#pragma once

namespace wde::scene {
	/**
	 * A class that represents a GameObject module type
	 */
	class Module {
		public:
			/**
			 * Creates a new Module type
			 * @param moduleName The name of the module
			 */
			explicit Module(const std::string moduleName) : _moduleName(std::move(moduleName)) {}
			virtual ~Module() = default;

			/** Initialize the module */
			virtual void initialize() = 0;
			/**
			 * Updates the module
			 * @param deltaTime Time since last update (in seconds)
			 */
			virtual void update(float deltaTime) = 0;
			/** Performs render operations given the command buffer */
			virtual void render(CommandBuffer& commandBuffer) = 0;
			/** Cleaning up the module */
			virtual void cleanUp() = 0;


		private:
			/** The unique identifier of the type of the module */
			std::string _moduleName;
	};
}

