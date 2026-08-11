#include "Gameplay/Plants/SunProducePlants/TwinSunflower.hpp"
PlantType TwinSunflower::getType() {
    return TWINSUNFLOWER;
}

void TwinSunflower::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    Vector2 spawnPos = { bounds.x + bounds.width * 0.5f, bounds.y };
    float targetY = bounds.y + bounds.height * 0.5f + GetRandomValue(-20, 20);
    spawnPos.x += GetRandomValue(-20, 20);
    
    mediator->spawnSun(spawnPos, targetY, 25);
    
    spawnPos.x += GetRandomValue(-30, 30);
    targetY += GetRandomValue(-30, 30);
    mediator->spawnSun(spawnPos, targetY, 25);

    resetCooldown();
    triggerBlush(1.0f);
}

void TwinSunflower::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}
