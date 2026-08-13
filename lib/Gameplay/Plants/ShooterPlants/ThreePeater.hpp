#pragma once
#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"
class ThreePeater : public ShooterPlant { 
public: 
    PlantType getType() override; 
    void performAction(IGameplayMediator* mediator) override;
};
