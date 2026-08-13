#include "Gameplay/Plants/ExplosivePlants/Squash.hpp"
PlantType Squash::getType() { return SQUASH; }

void Squash::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    Rectangle area = {
        bounds.x - bounds.width * 0.5f,
        bounds.y,
        bounds.width * 2.0f,
        bounds.height
    };

    if (readyToExplode) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, SQUASH);
        health = 0; // Die
    } else if (!animationStarted && mediator->hasZombieInArea(area)) {
        triggerActionAnimation();
        animationStarted = true;
        cooldownTimer = 999.0f; // Wait for animation to finish
    }
}
