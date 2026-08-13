#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"

ExplosivePlant::ExplosivePlant() : Plant() {}

void ExplosivePlant::onActionAnimationFinished() {
    readyToExplode = true;
}

void ExplosivePlant::performAction(IGameplayMediator* mediator) {
    // Default explosive plant action. Overridden by specific explosive plants.
}
