#pragma once
#include "Gameplay/Plants/MeleePlants/MeleePlant.hpp"

class Chomper : public MeleePlant { 
public: 
    PlantType getType() override; 
    void plantSetup() override;
    void updateTime(float dt) override;
    void performAction(IGameplayMediator* mediator) override;
};
