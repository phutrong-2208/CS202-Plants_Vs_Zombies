#include "Gameplay/Plants/ExplosivePlants/IceShroom.hpp"
PlantType IceShroom::getType() { return ICESHROOM; }

void IceShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // Whole screen area
    Rectangle area = {
        0.0f,
        0.0f,
        2000.0f,
        1500.0f
    };
    
    mediator->damageZombiesInArea(area, getDamage()); // Usually 20 damage
    mediator->freezeZombiesInArea(area, 4.0f); // Freeze for 4 seconds
    mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, ICESHROOM);
    health = 0; // Die
}
