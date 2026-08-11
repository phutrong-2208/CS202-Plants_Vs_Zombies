#ifndef SUNFLOWER_HPP
#define SUNFLOWER_HPP

#include "Gameplay/Plants/SunProducePlants/SunPlant.hpp"

class Sunflower : public SunPlant {
public:
    Sunflower();
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
    void plantSetup() override;
    void resetCooldown() override;
};

#endif
