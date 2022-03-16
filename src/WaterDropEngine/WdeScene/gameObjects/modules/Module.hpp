#pragma once

#include "../../../../wde.hpp"
#include "../../gizmo/Gizmo.hpp"

namespace wde::scene {
	class GameObject;

	/**
	 * A class that represents a GameObject module
	 */
	class Module : public NonCopyable {
		public:
			explicit Module(GameObject& gameObject, std::string name) : _gameObject(gameObject), _name(std::move(name)) {}
			~Module() override = default;

			// Inherited methods
			/** Tick for the module */
			virtual void tick() {};
			/** Draw the module GUI */
			virtual void drawGUI() {};
			/** Draw the gizmo elements to the scene */
			virtual void drawGizmo(Gizmo& gizmo, render::CommandBuffer& commandBuffer) {};


			// Getters and setters
			std::string getName() const { return _name; }


		protected:
			/** The game object owner of this module */
			GameObject& _gameObject;
			/** The module unique name */
			std::string _name;
	};
}
