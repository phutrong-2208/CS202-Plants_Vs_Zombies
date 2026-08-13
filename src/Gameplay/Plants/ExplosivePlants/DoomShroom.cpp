#include "Gameplay/Plants/ExplosivePlants/DoomShroom.hpp"
PlantType DoomShroom::getType() { return DOOMSHROOM; }

void DoomShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // 7x7 area
    Rectangle area = {
        bounds.x - bounds.width * 3.0f,
        bounds.y - bounds.height * 3.0f,
        bounds.width * 7.0f,
        bounds.height * 7.0f
    };
    
    mediator->damageZombiesInArea(area, getDamage());
    mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, DOOMSHROOM);
    health = 0; // Die
}
