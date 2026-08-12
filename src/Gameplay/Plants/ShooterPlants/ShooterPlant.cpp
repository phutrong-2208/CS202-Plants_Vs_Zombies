#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

ShooterPlant::ShooterPlant() = default;

Vector2 ShooterPlant::getProjectileSpawnPosition() {
    return {bounds.x + 108.0f, bounds.y + 45.0f};
}

void ShooterPlant::performAction(IGameplayMediator* mediator) {
    if (plantData && plantData->getProjectileCooldown() > 0.0f && getProjectileSpawnPosition().x != 0.0f) {
        if (!mediator->hasTarget(getType(), getProjectileSpawnPosition(), getBounds())) return;
        mediator->addProjectile(getType(), getProjectileSpawnPosition(), getDamage());
        resetCooldown();
    }
}
