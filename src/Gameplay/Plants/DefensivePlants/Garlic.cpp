#include "Gameplay/Plants/DefensivePlants/Garlic.hpp"
PlantType Garlic::getType() { return GARLIC; }

void Garlic::receiveDamage(int damage, Zombie* attacker, IGameplayMediator* mediator) {
    DefensivePlant::receiveDamage(damage, attacker, mediator);
    
    if (attacker && mediator) {
        mediator->changeZombieLane(attacker);
    }
}
