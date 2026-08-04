#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"

Sunflower::Sunflower() {
    resetCooldown();
}

PlantType Sunflower::getType() {
    return SUNFLOWER;
}

void Sunflower::resetCooldown() {
    if (plantData) {
        // Randomize cooldown by +/- 2 seconds
        cooldownTimer = plantData->getProjectileCooldown() + GetRandomValue(-200, 200) / 100.0f;
    }
}
