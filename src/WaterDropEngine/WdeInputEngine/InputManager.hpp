#pragma once

#include "../WdeCore/WdeModule.hpp"
#include "../WdeRenderingEngine/WdeRenderEngine.hpp"

namespace wde::inputManager {
	class InputManager : public WdeModule::Module<InputManager> {
		public:
			// Register module
			inline static const int MODULE_REGISTERED = RegisterModule("inputEngine", Stage::PRE);

			/** <"Key name", Corresponding key id> */
			typedef std::map<std::string, int> KeyMapping;


			// Core functions
			void initialize() override;
			void tick() override;
			void cleanUp() override;


			// Getters and setters
			/**
			 * @param keyName Name of the key preset in the file
			 * @return True if the key is being pressed
			 */
			bool isKeyDown(const std::string& keyName);


		private:
			/** The number of keys that needs to be setup */
			const int KEY_COUNT = 10;
			/** User defined key map */
			KeyMapping _userKeyMapping {};


			// Helpers
			/** Add key to the keymap list */
			void addKey(std::string keyName, int keyID) {
				if (_userKeyMapping.contains(keyName))
					throw WdeException("Two key strokes have the same name (" + keyName + ").", LoggerChannel::INPUT_MANAGER);
				_userKeyMapping.emplace(keyName, keyID);
			}

			// Core functions
			/** Sets the key binding to the default key binding */
			void setDefaultKeyMapping();
	};
}

