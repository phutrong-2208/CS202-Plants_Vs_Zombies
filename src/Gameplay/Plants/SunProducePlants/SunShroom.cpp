#include "Gameplay/Plants/SunProducePlants/SunShroom.hpp"
PlantType SunShroom::getType() {
    return SUNSHROOM;
}

void SunShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    Vector2 spawnPos = { bounds.x + bounds.width * 0.5f, bounds.y };
    float targetY = bounds.y + bounds.height * 0.5f + GetRandomValue(-20, 20);
    spawnPos.x += GetRandomValue(-20, 20);
    
    // TODO: SunShroom usually drops smaller sun (15) then grows, but for now we follow old logic of 25.
    mediator->spawnSun(spawnPos, targetY, 25);
    resetCooldown();
    triggerBlush(1.0f);
}

void SunShroom::plantSetup() {
    Plant::plantSetup();
    // First sun is spawned after 4-8 seconds
    cooldownTimer = GetRandomValue(400, 800) / 100.0f;
}
