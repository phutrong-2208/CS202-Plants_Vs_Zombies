#include "Gameplay/Plants/MeleePlants/SpikePlant.hpp"

PlantType SpikePlant::getType() { return type; }

void SpikePlant::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    if (plantData && plantData->getCooldown() > 0.0f) {
        Rectangle damageArea = getBounds();
        
        // Slightly expand the area to catch zombies stepping on it
        damageArea.x -= 10.0f;
        damageArea.width += 20.0f;
        
        if (mediator->hasZombieInArea(damageArea)) {
            mediator->damageZombiesInArea(damageArea, getDamage());
            resetCooldown();
        }
    }
}
