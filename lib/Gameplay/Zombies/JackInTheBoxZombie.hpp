#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class JackInTheBoxZombie : public Zombie {
private:
    float explodeTimer = 0.0f;
    bool isPopping = false;
    float popTimer = 0.0f;

protected:
    void zombieSetup() override;
    void onCustomCombat(float dt, IGameplayMediator& mediator) override;

public:
    JackInTheBoxZombie() = default;
    void updateTime(float dt) override;
};
