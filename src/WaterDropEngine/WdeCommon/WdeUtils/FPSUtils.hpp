#pragma once

#pragma once

#include <chrono>

namespace wde {
	/**
	 * Manage the engine FPS data
	 */
	class FPSUtils {
		public:
			/** Called when the next frame is done being drawn on the screen */
			void update() {
				WDE_PROFILE_FUNCTION();
				// Computes delta time
				auto newTime = std::chrono::steady_clock::now();
				auto elapsedTime =
						std::chrono::time_point_cast<std::chrono::milliseconds>(newTime).time_since_epoch()
						- std::chrono::time_point_cast<std::chrono::milliseconds>(_lastTime).time_since_epoch();

				if (elapsedTime.count() >= 1000) { // One second passed
					// Update new time
					_lastTime = newTime;

					// Add FPS
					_lastFPSValue = _currentFPSCount;
					_currentFPSCount = 0;
					_newValue = true;
				}
				_currentFPSCount++;
			}

			/** @return the actual average FPS */
			int getFPS() const { return _lastFPSValue; }
			bool hasNewValue() {
				if (_newValue) {
					_newValue = false;
					return true;
				}
				return false;
			}


		private:
			int _lastFPSValue = 0;
			int _currentFPSCount = 0;
			bool _newValue = false;
			std::chrono::time_point<std::chrono::steady_clock> _lastTime = std::chrono::steady_clock::now();
	};
}


