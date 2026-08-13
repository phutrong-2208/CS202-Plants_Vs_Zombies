#include "Gameplay/Plants/ExplosivePlants/PotatoMine.hpp"
PlantType PotatoMine::getType() { return POTATOMINE; }

void PotatoMine::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // PotatoMine needs a zombie in its 1x1 cell to trigger
    Rectangle area = {
        bounds.x,
        bounds.y,
        bounds.width,
        bounds.height
    };
    
    if (mediator->hasZombieInArea(area)) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, POTATOMINE);
        health = 0; // Die
    }
}
