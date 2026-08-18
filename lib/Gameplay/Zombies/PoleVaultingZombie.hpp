#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class PoleVaultingZombie : public Zombie {
private:
    bool hasVaulted = false;
    bool isVaulting = false;
    float vaultProgress = 0.0f;
    float vaultDuration = 1.3f;
    float vaultOffsetX = 0.0f;
    float vaultOffsetY = 0.0f;
    bool hasDroppedPoleParticle = false;
    IGameplayMediator* mediatorRef = nullptr;

protected:
    void zombieSetup() override;
    void onCustomCombat(float dt, IGameplayMediator& mediator) override;

public:
    PoleVaultingZombie() = default;
    void updateTime(float dt) override;
    Rectangle getHitbox() const override;
};
