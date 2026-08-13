#include "Gameplay/Plants/DefensivePlants/HypnoShroom.hpp"
PlantType HypnoShroom::getType() { return HYPNOSHROOM; }

void HypnoShroom::receiveDamage(int damage, Zombie* attacker, IGameplayMediator* mediator) {
    DefensivePlant::receiveDamage(damage, attacker, mediator);
    
    if (attacker && mediator) {
        mediator->hypnotizeZombie(attacker);
        health = 0; // Die immediately upon being eaten
    }
}
