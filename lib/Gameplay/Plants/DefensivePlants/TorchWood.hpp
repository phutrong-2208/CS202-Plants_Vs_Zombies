#ifndef TORCHWOOD_HPP
#define TORCHWOOD_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class TorchWood : public DefensivePlant {
public:
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
