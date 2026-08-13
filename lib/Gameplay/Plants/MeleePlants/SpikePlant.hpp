#ifndef SPIKEPLANT_HPP
#define SPIKEPLANT_HPP

#include "Gameplay/Plants/MeleePlants/MeleePlant.hpp"

class SpikePlant : public MeleePlant {
private:
    PlantType type;
public:
    SpikePlant(PlantType t) : type(t) {}
    PlantType getType() override;
    void performAction(IGameplayMediator* mediator) override;
};

#endif
