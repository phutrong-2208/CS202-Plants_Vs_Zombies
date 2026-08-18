#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class ZomboniZombie : public Zombie {
protected:
    void zombieSetup() override;
    void onCustomCombat(float dt, IGameplayMediator& mediator) override;

public:
    ZomboniZombie() = default;
};
