#pragma once

#include <string>
#include "../WdeLogger/LoggerHandler.hpp"
#include "../WdeException/WdeException.hpp"

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


			// Operators overload
			/** Scalar product */
			friend Color operator*(float a, const Color &v) {
				return {v._r * a, v._g * a, v._b * a, v._a * a};
			}

			Color &operator/=(float a) {
				if (a == 0)
					throw WdeException(LogChannel::RENDER, "Cannot divide a Vector by 0.");
				_r /= a;
				_g /= a;
				_b /= a;
				_a /= a;
				return *this;
			}

			Color &operator*=(float a) {
				_r *= a;
				_g *= a;
				_b *= a;
				_a *= a;
				return *this;
			}

			Color &operator-=(const Color &v) {
				_r -= v._r;
				_g -= v._g;
				_b -= v._b;
				_a -= v._a;
				return *this;
			}

			Color &operator+=(const Color &v) {
				_r += v._r;
				_g += v._g;
				_b += v._b;
				_a += v._a;
				return *this;
			}

			friend Color operator/(const Color &v, float a) {
				if (a == 0)
					throw WdeException(LogChannel::RENDER, "Cannot divide a Vector by 0.");
				return {v._r / a, v._g / a, v._b / a, v._a / a};
			}

			friend Color operator/(float a, const Color &v) {
				if (a == 0)
					throw WdeException(LogChannel::RENDER, "Cannot divide a Vector by 0.");
				return {v._r / a, v._g / a, v._b / a, v._a / a};
			}

			friend Color operator*(const Color &v, float a) {
				return {v._r * a, v._g * a, v._b * a, v._a * a};
			}

			friend Color operator*(const Color &v1, const Color &v2) {
				return {v1._r * v2._r, v1._g * v2._g, v1._b * v2._b, v1._a * v2._a};
			}

			friend Color operator-(const Color &v1, const Color &v2) {
				return {v1._r - v2._r, v1._g - v2._g, v1._b - v2._b, v1._a - v2._a};
			}

			friend Color operator+(const Color &v1, const Color &v2) {
				return {v1._r + v2._r, v1._g + v2._g, v1._b + v2._b, v1._a + v2._a};
			}
	};
}
