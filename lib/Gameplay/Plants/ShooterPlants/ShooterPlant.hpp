#ifndef SHOOTERPLANT_HPP
#define SHOOTERPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class ShooterPlant : public Plant {
protected:
    float fireDelayTimer = 0.0f;
    bool readyToFire = false;
    float resetAnimTimer = 0.0f;
public:
    ShooterPlant();

    virtual Vector2 getProjectileSpawnPosition();
    void onActionAnimationFinished() override;
    void performAction(IGameplayMediator* mediator) override;
    void updateTime(float dt) override;
};

#endif
