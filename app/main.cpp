#include "CoreApp.hpp"
#include "examples/ExamplesApp.hpp"

int main() {
	// Use custom app
    // CoreApp app {};

    // Use examples
    ExamplesApp app {};

    app.start();

    return 0;
}

