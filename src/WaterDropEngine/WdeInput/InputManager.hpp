#pragma once

#include "../../wde.hpp"
#include <map>

namespace wde::input {
	class InputManager : public NonCopyable {
		public:
			/** <"Key name", Corresponding key id> */
			typedef std::map<std::string, int> KeyMapping;


			// Core functions
			explicit InputManager();
			~InputManager() override;
			void tick() {};


			// Getters and setters
			/**
			 * @param keyName Name of the key preset in the file
			 * @return True if the key is being pressed
			 */
			bool isKeyDown(const std::string& keyName) const;


		private:
			/** The number of keys that needs to be setup */
			const int KEY_COUNT = 10;
			/** User defined key map */
			KeyMapping _userKeyMapping {};


			// Helpers
			/** Add key to the keymap list */
			void addKey(const std::string& keyName, int keyID);

			// Core functions
			/** Sets the key binding to the default key binding */
			void setDefaultKeyMapping();
	};
}