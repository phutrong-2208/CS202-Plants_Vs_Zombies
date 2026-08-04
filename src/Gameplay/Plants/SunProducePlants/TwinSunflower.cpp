#include "Gameplay/Plants/SunProducePlants/TwinSunflower.hpp"
PlantType TwinSunflower::getType() { return TWINSUNFLOWER; }

void TwinSunflower::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}
