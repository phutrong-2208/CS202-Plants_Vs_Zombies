#pragma once
#include "Gameplay/Plants/SunProducePlants/SunPlant.hpp"
class SunShroom : public SunPlant {
public:
    PlantType getType() override;
    void plantSetup() override;
    void performAction(IGameplayMediator* mediator) override;
};
