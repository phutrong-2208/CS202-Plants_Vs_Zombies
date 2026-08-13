#ifndef MAGNETSHROOM_HPP
#define MAGNETSHROOM_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class MagnetShroom : public DefensivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
