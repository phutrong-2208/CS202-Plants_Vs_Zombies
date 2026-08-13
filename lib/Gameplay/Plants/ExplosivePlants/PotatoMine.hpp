#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class PotatoMine : public ExplosivePlant {
private:
    float armTimer = 15.0f;
    bool armed = false;
public:
    PlantType getType() override;
    void updateTime(float deltaSeconds) override;
    void performAction(IGameplayMediator* mediator) override;
};
