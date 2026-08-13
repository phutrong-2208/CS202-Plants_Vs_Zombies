#ifndef BLOVER_HPP
#define BLOVER_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class Blover : public DefensivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
