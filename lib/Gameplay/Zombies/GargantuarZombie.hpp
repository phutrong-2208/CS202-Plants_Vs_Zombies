#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class GargantuarZombie : public Zombie {
private:
    bool hasThrownImp = false;
    bool isThrowing = false;
    float throwTimer = 0.0f;
    bool impSpawned = false;

    bool isSmashing = false;
    float smashTimer = 0.0f;
    float smashCooldown = 0.0f;
    bool smashDealt = false;
    IGameplayMediator* lastMediator = nullptr;

protected:
    void zombieSetup() override;
    void onCustomCombat(float dt, IGameplayMediator& mediator) override;

public:
    GargantuarZombie() = default;
    void updateTime(float dt) override;
    void receiveDamage(float damage, IGameplayMediator* mediator = nullptr) override;
};
