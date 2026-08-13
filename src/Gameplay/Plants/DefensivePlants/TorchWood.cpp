#include "Gameplay/Plants/DefensivePlants/TorchWood.hpp"
PlantType TorchWood::getType() { return TORCHWOOD; }

void TorchWood::performAction(IGameplayMediator* mediator) {
    if (mediator) {
        mediator->tryIgniteProjectile(bounds);
    }
}
