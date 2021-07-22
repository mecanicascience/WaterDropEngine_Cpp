#pragma once

namespace wde {
	class Color {
		public:
			// Core parameters
			float _r;
			float _g;
			float _b;
			float _a;

			// Constructors
			Color(float r, float g, float b) : Color(r, g, b, 1) {}
			Color(float r, float g, float b, float a) : _r(r), _g(g), _b(b), _a(a) {}
	};
}
