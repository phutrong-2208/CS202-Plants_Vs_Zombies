#pragma once
#include "Gameplay/Plants/SunProducePlants/SunPlant.hpp"
class Marigold : public SunPlant { 
public: 
    Marigold();
    PlantType getType() override; 
    void performAction(IGameplayMediator* mediator) override;
    void plantSetup() override;
    void resetCooldown() override;
};
