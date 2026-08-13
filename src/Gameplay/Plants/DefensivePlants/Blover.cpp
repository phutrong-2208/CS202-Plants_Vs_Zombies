#include "Gameplay/Plants/DefensivePlants/Blover.hpp"
PlantType Blover::getType() { return BLOVER; }

void Blover::performAction(IGameplayMediator* mediator) {
    if (mediator && !isOnCooldown()) {
        mediator->killZombiesOfType(BALLOON_ZOMBIE);
        health = 0; // Blover typically destroys itself after blowing away zombies.
    }
}
