#ifndef REPEATER_HPP
#define REPEATER_HPP

#include "Gameplay/Plants/Plant.hpp"

class Repeater : public Plant {
public:
    Repeater();
    PlantType getType() override;
    Vector2 getProjectileSpawnPosition() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
