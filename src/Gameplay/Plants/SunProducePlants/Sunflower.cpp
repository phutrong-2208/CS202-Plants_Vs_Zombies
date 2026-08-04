#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"

Sunflower::Sunflower() {
    resetCooldown();
}

PlantType Sunflower::getType() {
    return SUNFLOWER;
}

void Sunflower::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}

void Sunflower::resetCooldown() {
    if (plantData) {
        // Subsequent suns spawn around the base cooldown (24 seconds) +/- 1s
        cooldownTimer = plantData->getProjectileCooldown() + GetRandomValue(-100, 100) / 100.0f;
    }
}
