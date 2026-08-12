#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

DefensivePlant::DefensivePlant() : Plant() {}

void DefensivePlant::performAction(IGameplayMediator* mediator) {
    // Defensive plants usually don't have active actions like shooting,
    // they just passively defend or have specific triggers handled elsewhere.
}
