#include "Gameplay/Plants/ExplosivePlants/PotatoMine.hpp"
PlantType PotatoMine::getType() { return POTATOMINE; }

void PotatoMine::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // PotatoMine needs a zombie in its cell to trigger, expand slightly to catch zombies eating it from the edge
    Rectangle area = {
        bounds.x - 15.0f,
        bounds.y,
        bounds.width + 30.0f,
        bounds.height
    };
    
    if (mediator->hasZombieInArea(area)) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, POTATOMINE);
        health = 0; // Die
    }
}
