#pragma once

#include "../../../wde.hpp"
#include "../gizmo/Gizmo.hpp"

namespace wde::scene {
	class GameObject;

	/**
	 * A class that represents a GameObject module
	 */
	class Module : public NonCopyable {
		public:
			explicit Module(GameObject& gameObject, std::string name) : _gameObject(gameObject), _name(std::move(name)), _icon("") {}
			explicit Module(GameObject& gameObject, std::string name, std::string icon) : _gameObject(gameObject), _name(std::move(name)), _icon(std::move(icon)) {}
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
			std::string getIcon() const { return _icon; }


		protected:
			/** The game object owner of this module */
			GameObject& _gameObject;
			/** The module unique name */
			std::string _name;
			/** The module icon */
			std::string _icon;
	};
}
