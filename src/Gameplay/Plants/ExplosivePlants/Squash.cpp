#include "Gameplay/Plants/ExplosivePlants/Squash.hpp"
PlantType Squash::getType() { return SQUASH; }

void Squash::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // Squash needs a zombie in its 1x1 cell or the one immediately ahead
    Rectangle area = {
        bounds.x - bounds.width * 0.5f,
        bounds.y,
        bounds.width * 2.0f,
        bounds.height
    };
    
    if (mediator->hasZombieInArea(area)) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, SQUASH);
        health = 0; // Die
    }
}
