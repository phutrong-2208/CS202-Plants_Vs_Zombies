#include "Gameplay/Zombies/ZomboniZombie.hpp"

void ZomboniZombie::zombieSetup() {
    Zombie::zombieSetup();
    speed = 20.0f;
}

void ZomboniZombie::onCustomCombat(float dt, IGameplayMediator& mediator) {
    if (state != ZombieState::WALKING) return;

    // Instantly crush any plant in front of the vehicle
    if (mediator.hasPlantInArea(getAttackHitbox())) {
        mediator.killPlantsInArea(getAttackHitbox());
    }
}
