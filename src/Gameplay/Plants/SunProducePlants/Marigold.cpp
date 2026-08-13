#include "Gameplay/Plants/SunProducePlants/Marigold.hpp"
Marigold::Marigold() {
    resetCooldown();
}

PlantType Marigold::getType() { return MARIGOLD; }

void Marigold::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    Vector2 spawnPos = { bounds.x + bounds.width * 0.5f, bounds.y };
    float targetY = bounds.y + bounds.height * 0.5f + GetRandomValue(-20, 20);
    spawnPos.x += GetRandomValue(-20, 20);
    
    mediator->spawnSun(spawnPos, targetY, 25);
    resetCooldown();
    triggerBlush(1.0f);
}

void Marigold::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}

void Marigold::resetCooldown() {
    if (plantData) {
        // Subsequent suns spawn around the base cooldown +/- 1s
        cooldownTimer = plantData->getCooldown() + GetRandomValue(-100, 100) / 100.0f;
    }
}
