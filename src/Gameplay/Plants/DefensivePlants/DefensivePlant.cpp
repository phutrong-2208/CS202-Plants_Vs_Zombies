#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

DefensivePlant::DefensivePlant() : Plant() {}

void DefensivePlant::updateTime(float deltaSeconds) {
    Plant::updateTime(deltaSeconds);

    if (plantData) {
        float maxHealth = plantData->getBaseHealth();
        if (maxHealth > 0) {
            float p = health / maxHealth;
            if (p <= 0.333f) {
                animation.setTextureOverrides(plantData->getCracked2Override());
            } else if (p <= 0.666f) {
                animation.setTextureOverrides(plantData->getCracked1Override());
            } else {
                animation.clearTextureOverrides();
            }
        }
    }
}

void DefensivePlant::receiveDamage(int damage, Zombie* attacker, IGameplayMediator* mediator) {
    Plant::receiveDamage(damage, attacker, mediator);
}

void DefensivePlant::performAction(IGameplayMediator* mediator) {
    // Defensive plants usually don't have active actions like shooting,
    // they just passively defend or have specific triggers handled elsewhere.
}
