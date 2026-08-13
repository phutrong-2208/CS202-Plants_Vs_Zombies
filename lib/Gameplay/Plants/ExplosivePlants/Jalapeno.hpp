#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class Jalapeno : public ExplosivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};
