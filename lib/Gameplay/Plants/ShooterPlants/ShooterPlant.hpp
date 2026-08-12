#ifndef SHOOTERPLANT_HPP
#define SHOOTERPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class ShooterPlant : public Plant {
public:
    ShooterPlant();

    virtual Vector2 getProjectileSpawnPosition();
    virtual void performAction(IGameplayMediator* mediator) override;
};

#endif
