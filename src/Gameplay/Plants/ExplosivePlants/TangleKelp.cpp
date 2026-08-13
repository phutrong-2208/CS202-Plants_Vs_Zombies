#include "Gameplay/Plants/ExplosivePlants/TangleKelp.hpp"
PlantType TangleKelp::getType() { return TANGLEKELP; }

void TangleKelp::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // 1x1 area
    Rectangle area = {
        bounds.x,
        bounds.y,
        bounds.width,
        bounds.height
    };
    
    if (mediator->hasZombieInArea(area)) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, TANGLEKELP);
        health = 0; // Die
    }
}
