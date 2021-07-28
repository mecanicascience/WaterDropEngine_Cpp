#pragma once

namespace wde::renderEngine {
    /**
     * Manage the engine FPS data
     */
    class FPSUtils {
        public:
            /** Called when the next frame is done beeing drawn on the screen */
            static void update() {
                // Computes delta time
                auto newTime = std::chrono::steady_clock::now();
                auto elapsedTime =
                        std::chrono::time_point_cast<std::chrono::milliseconds>(newTime).time_since_epoch()
                        - std::chrono::time_point_cast<std::chrono::milliseconds>(_lastTime).time_since_epoch();

                if (elapsedTime.count() >= 1000.0f) { // One second passed
                    // Update new time
                    _lastTime = newTime;

                    // Add FPS
                    _lastFPSValue = _currentFPSCount;
                    _currentFPSCount = 0;
                }
                _currentFPSCount++;
            }

            /** @return the actual average FPS */
            static int getFPS() { return _lastFPSValue; }


        private:
            static int _lastFPSValue;
            static int _currentFPSCount;
            static std::chrono::time_point<std::chrono::steady_clock> _lastTime;
    };


    // Initial variables values
    int FPSUtils::_lastFPSValue = 0;
    int FPSUtils::_currentFPSCount = 0;
    std::chrono::time_point<std::chrono::steady_clock> FPSUtils::_lastTime = std::chrono::steady_clock::now();
}
