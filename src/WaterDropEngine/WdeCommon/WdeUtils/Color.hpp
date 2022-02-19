#pragma once

namespace wde {
	/**
	 * Stores a color
	 */
	class Color {
		public:
			// Core parameters
			float _r;
			float _g;
			float _b;
			float _a;

			// Constructors
			explicit Color(float r, float g, float b) : Color(r, g, b, 1) {}
			explicit Color(float r, float g, float b, float a) : _r(r), _g(g), _b(b), _a(a) {}


			std::string toString() {
				return "(" + std::to_string(int(_r * 255.0))
					+ "," + std::to_string(int(_g * 255.0))
					+ "," + std::to_string(int(_b * 255.0))
					+ "," + std::to_string(int(_a * 255.0)) + ")";
			}
	};
}

