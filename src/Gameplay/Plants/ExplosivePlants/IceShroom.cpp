#include "Gameplay/Plants/ExplosivePlants/IceShroom.hpp"
PlantType IceShroom::getType() { return ICESHROOM; }

void IceShroom::updateTime(float deltaSeconds) {
    Plant::updateTime(deltaSeconds);
    if (animationStarted && !readyToExplode && health > 0) {
        fuseTimer -= deltaSeconds;
        if (fuseTimer <= 0.0f) {
            readyToExplode = true;
        }
    }
}

void IceShroom::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    if (readyToExplode) {
        // Screen-wide effect freezing all zombies across the entire lawn
        Rectangle screenArea = {-200.0f, -200.0f, 1800.0f, 1200.0f};
        
        mediator->damageZombiesInArea(screenArea, getDamage()); // 20 damage
        mediator->freezeZombiesInArea(screenArea, 5.0f); // Freeze all zombies for 5 seconds
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, ICESHROOM);
        mediator->playSound("FROZEN", 1.0f);
        health = 0; // Die
    } else if (!animationStarted) {
        animationStarted = true;
    }
}
