#include "Gameplay/Plants/ExplosivePlants/Jalapeno.hpp"
PlantType Jalapeno::getType() { return JALAPENO; }

void Jalapeno::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    if (readyToExplode) {
        // Whole lane area
        Rectangle area = {
            0.0f,
            bounds.y,
            1500.0f, // Assume screen width is enough
            bounds.height
        };
        
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, JALAPENO);
        health = 0; // Die
    } else if (!animationStarted) {
        triggerActionAnimation();
        animationStarted = true;
    }
}
