#include "Gameplay/Plants/ExplosivePlants/CherryBomb.hpp"
PlantType CherryBomb::getType() { return CHERRYBOMB; }

void CherryBomb::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // 3x3 area
    Rectangle area = {
        bounds.x - bounds.width,
        bounds.y - bounds.height,
        bounds.width * 3.0f,
        bounds.height * 3.0f
    };
    
    mediator->damageZombiesInArea(area, getDamage());
    mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, CHERRYBOMB);
    health = 0; // Die
}
