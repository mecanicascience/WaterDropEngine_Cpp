#include "CoreApp.hpp"

#include "../src/WaterDropEngine/WdeCommon/WdeError/WdeException.hpp"

int main() {
    CoreApp app {};
    WdeStatus status = app.initialize();

    return WdeException::toInt(status);
}

