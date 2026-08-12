#ifndef REPEATER_HPP
#define REPEATER_HPP

#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

class Repeater : public ShooterPlant {
public:
    Repeater();
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
