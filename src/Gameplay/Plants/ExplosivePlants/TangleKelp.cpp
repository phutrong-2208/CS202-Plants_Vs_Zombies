#include "Gameplay/Plants/ExplosivePlants/TangleKelp.hpp"
PlantType TangleKelp::getType() { return TANGLEKELP; }

void TangleKelp::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // 1x1 area, expanded slightly to catch edge zombies
    Rectangle area = {
        bounds.x - 15.0f,
        bounds.y,
        bounds.width + 30.0f,
        bounds.height
    };
    
    if (mediator->hasZombieInArea(area)) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, TANGLEKELP);
        health = 0; // Die
    }
}
