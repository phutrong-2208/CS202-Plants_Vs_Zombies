#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class Squash : public ExplosivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};
