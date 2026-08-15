#include "Gameplay/Plants/ExplosivePlants/PotatoMine.hpp"
PlantType PotatoMine::getType() { return POTATOMINE; }

void PotatoMine::updateTime(float deltaSeconds) {
    Plant::updateTime(deltaSeconds);
    
    if (!armed && health > 0) {
        armTimer -= deltaSeconds;
        if (armTimer <= 0.0f) {
            armed = true;
            animation.setDefaultClip("anim_armed");
        }
    }
}

void PotatoMine::performAction(IGameplayMediator* mediator) {
    if (!mediator || !armed) return;
    
    // PotatoMine needs a zombie in its cell to trigger, expand slightly to catch zombies eating it from the edge
    Rectangle area = {
        bounds.x - 15.0f,
        bounds.y,
        bounds.width + 30.0f,
        bounds.height
    };
    
    if (readyToExplode) {
        mediator->damageZombiesInArea(area, getDamage());
        mediator->spawnExplosionParticles({bounds.x + bounds.width * 0.5f, bounds.y + bounds.height * 0.5f}, POTATOMINE);
        health = 0; // Die
    } else if (mediator->hasZombieInArea(area) && !animationStarted) {
        triggerAnimation("anim_mashed"); // Mashed is the explode animation for PotatoMine
        animationStarted = true;
    }
}
