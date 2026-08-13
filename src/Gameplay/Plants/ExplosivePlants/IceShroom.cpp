#include "Gameplay/Plants/ExplosivePlants/IceShroom.hpp"
PlantType IceShroom::getType() { return ICESHROOM; }

void IceShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // 3x3 area
    Rectangle area = {
        bounds.x - bounds.width,
        bounds.y - bounds.height,
        bounds.width * 3.0f,
        bounds.height * 3.0f
    };
    
    mediator->damageZombiesInArea(area, getDamage()); // Usually 20 damage
    mediator->freezeZombiesInArea(area, 4.0f); // Freeze for 4 seconds
    mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, ICESHROOM);
    health = 0; // Die
}
