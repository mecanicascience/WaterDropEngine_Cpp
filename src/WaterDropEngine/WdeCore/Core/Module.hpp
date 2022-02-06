#pragma once

#include "../../../wde.hpp"
#include "../Structure/Subject.hpp"

namespace wde::core {
	/**
	 * Represents a game module.
	 * Each game module is subscribed to the modules subject as an observer.
	 */
	class Module : public core::Observer {
		public:
			/**
			 * Create a new module
			 * @param moduleSubject The global subject common to every modules
			 */
			explicit Module(std::shared_ptr<core::Subject> moduleSubject) : _moduleSubject(std::move(moduleSubject)) {}

			/** Called on every game tick */
			virtual void tick() = 0;
			/** Called when the module must end and clean up resources */
			virtual void cleanUp() = 0;


		protected:
			/** The subject that the module is registered to */
			std::shared_ptr<core::Subject> _moduleSubject;
	};
}
