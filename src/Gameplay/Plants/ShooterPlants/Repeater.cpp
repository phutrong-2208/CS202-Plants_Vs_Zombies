#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"

Repeater::Repeater() = default;

PlantType Repeater::getType() {
    return REPEATER;
}

void Repeater::performAction(IGameplayMediator* mediator) {
    if (plantData && plantData->getProjectileCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        if (!mediator->hasTarget(getType(), getProjectileSpawnPosition(), getBounds())) return;
        
        Vector2 spawnPos = getProjectileSpawnPosition();
        
        // Spawn first pea
        mediator->addProjectile(getType(), spawnPos, getDamage());
        
        // Spawn second pea with a slight X offset
        Vector2 secondPos = spawnPos;
        secondPos.x -= 30.0f; 
        
        mediator->addProjectile(getType(), secondPos, getDamage());
        
        resetCooldown();
    }
}
