#include "CoreApp.hpp"

int main() {
    CoreApp app {};

    WdeStatus status = app.initialize();

    return WdeException::toInt(status);
}

