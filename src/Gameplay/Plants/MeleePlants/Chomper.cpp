#include "Gameplay/Plants/MeleePlants/Chomper.hpp"
#include "Gameplay/Zombies/Zombie.hpp"

PlantType Chomper::getType() { return CHOMPER; }

void Chomper::plantSetup() {
    Plant::plantSetup();
    cooldownTimer = 0.0f; // Ready to eat immediately
}

void Chomper::updateTime(float dt) {
    bool wasOnCooldown = isOnCooldown();
    Plant::updateTime(dt);
    
    // If we were digesting (on cooldown) and just finished, return to idle
    if (wasOnCooldown && !isOnCooldown()) {
        animation.resetToDefault();
    }
}

void Chomper::performAction(IGameplayMediator* mediator) {
    if (!mediator) return;
    
    // Only act if not digesting (cooldown == 0)
    if (plantData && cooldownTimer <= 0.0f) {
        Rectangle sensor = {bounds.x + 20.0f, bounds.y, bounds.width * 1.5f - 20.0f, bounds.height};
        
        Zombie* target = mediator->getZombiePriority(sensor);
        if (target) {
            target->setSwallowed(true);
            target->receiveDamage(10000.0f); // Instant kill
            
            // Start digesting
            resetCooldown(); 
            animation.playClip("chew"); // Play chew or swallow clip
            animation.setLoopToggle(true); // Loop it while digesting
        }
    }
}
