#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class IceShroom : public ExplosivePlant {
private:
    float fuseTimer = 1.0f;
public:
    PlantType getType() override;
    void updateTime(float deltaSeconds) override;
    void performAction(IGameplayMediator* mediator) override;
};
