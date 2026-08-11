#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"

Repeater::Repeater() = default;

PlantType Repeater::getType() {
    return REPEATER;
}

Vector2 Repeater::getProjectileSpawnPosition() {
    return {bounds.x + 108.0f, bounds.y + 45.0f};
}

void Repeater::performAction(IGameplayMediator* mediator) {
    if (plantData && plantData->getProjectileCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        if (!mediator->hasTarget(getType(), getProjectileSpawnPosition(), getBounds())) return;
        
        Vector2 spawnPos = getProjectileSpawnPosition();
        
        // Spawn first pea
        mediator->addProjectile(getType(), spawnPos, getDamage());
        
        // Spawn second pea with a slight X offset so they don't overlap completely,
        // or a small delay. A simple X offset behind the first one gives the illusion of sequence!
        Vector2 secondPos = spawnPos;
        secondPos.x -= 30.0f; // 30 pixels behind
        
        mediator->addProjectile(getType(), secondPos, getDamage());
        
        resetCooldown();
    }
}
