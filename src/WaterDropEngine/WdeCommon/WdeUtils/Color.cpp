#include "Color.hpp"

namespace wde {
	// Default colors
	Color Color::BLACK = Color(0, 0, 0, 1);
	Color Color::GREY  = Color(0.5, 0.5, 0.5, 1);
	Color Color::WHITE = Color(1, 1, 1, 1);
	Color Color::CLEAR = Color(0, 0, 0, 0);

	Color Color::RED   = Color(1, 0, 0, 1);
	Color Color::GREEN = Color(0, 1, 0, 1);
	Color Color::BLUE  = Color(0, 0, 1, 1);

	Color Color::CYAN    = Color(0, 1, 1, 1);
	Color Color::MAGENTA = Color(1, 0, 1, 1);
	Color Color::YELLOW  = Color(1, 0.92, 0.016, 1);
}
