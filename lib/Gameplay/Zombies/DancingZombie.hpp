#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class DancingZombie : public Zombie {
private:
    float summonCooldown = 6.0f;
    bool isSummoning = false;
    float summonTimer = 0.0f;

protected:
    void zombieSetup() override;
    void onCustomCombat(float dt, IGameplayMediator& mediator) override;

public:
    DancingZombie() = default;
    void updateTime(float dt) override;
};
