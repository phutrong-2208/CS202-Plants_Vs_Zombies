#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class DoomShroom : public ExplosivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};
