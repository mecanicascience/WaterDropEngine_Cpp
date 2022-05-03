#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
/**
 * Converts a number from degrees to radians
 * @param degrees
 * @return The number in radius
 */
inline double degreesToRadians(double degrees) {
	return degrees * pi / 180.0;
}

/** @return A random double number */
inline double randomDouble() {
	static std::uniform_real_distribution<double> distribution (0.0, 1.0);
	static std::mt19937 generator {};
	return distribution(generator);
}

inline double randomDouble(double min, double max) {
	static std::uniform_real_distribution<double> distribution (0.0, 1.0);
	static std::mt19937 generator {};
	return distribution(generator) * (max - min) + min;
}

/**
 * Clamp a parameter between two values
 * @param x The parameter to clamp
 * @param min
 * @param max
 * @return The clamped number
 */
inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// Common Headers
#include "../rays/Ray.hpp"
#include "Vector.hpp"
