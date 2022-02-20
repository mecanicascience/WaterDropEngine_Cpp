#pragma once

#include "../../wde.hpp"
#include <map>

namespace wde::input {
	class InputManager {
		public:
			/** <"Key name", Corresponding key id> */
			typedef std::map<std::string, int> KeyMapping;


			// Core functions
			explicit InputManager();
			~InputManager();
			void tick() {};


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
					throw WdeException(LogChannel::INPUT, "Two key strokes have the same name (" + keyName + ").");
				_userKeyMapping.emplace(keyName, keyID);
			}

			// Core functions
			/** Sets the key binding to the default key binding */
			void setDefaultKeyMapping();
	};
}