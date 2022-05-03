#pragma once

#include <string>

namespace wde {
	/**
	 * Stores a color
	 */
	class Color {
		public:
			// Default colors
			static Color BLACK;
			static Color GREY;
			static Color WHITE;
			static Color CLEAR;

			static Color RED;
			static Color GREEN;
			static Color BLUE;

			static Color CYAN;
			static Color MAGENTA;
			static Color YELLOW;


			// Core parameters
			float _r;
			float _g;
			float _b;
			float _a;

			// Constructors
			Color(double r, double g, double b, double a = 1) : _r(static_cast<float>(r)), _g(static_cast<float>(g)), _b(static_cast<float>(b)), _a(static_cast<float>(a)) {}
			Color(int r, int g, int b, int a = 1) : _r(static_cast<float>(r)), _g(static_cast<float>(g)), _b(static_cast<float>(b)), _a(static_cast<float>(a)) {}
			Color(float r, float g, float b, float a = 1) : _r(r), _g(g), _b(b), _a(a) {}


			std::string toString() const {
				return "(" + std::to_string(int(_r * 255.0))
				       + "," + std::to_string(int(_g * 255.0))
				       + "," + std::to_string(int(_b * 255.0))
				       + "," + std::to_string(int(_a * 255.0)) + ")";
			}


			typedef unsigned char uchar;
			/** @return the 2 bytes rgba value of the color */
			uint32_t toRGBA2B() const {
				return  uchar(_r * 255)
					 + (uchar(_g * 255) << 8)
			         + (uchar(_b * 255) << 16)
				     + (uchar(_a * 255) << 24);
			}
	};
}
