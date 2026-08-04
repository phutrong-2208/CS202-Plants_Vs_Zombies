#include "Gameplay/Plants/SunProducePlants/SunShroom.hpp"
PlantType SunShroom::getType() { return SUNSHROOM; }

void SunShroom::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}
