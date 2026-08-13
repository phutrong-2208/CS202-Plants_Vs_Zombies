#ifndef MELEEPLANT_HPP
#define MELEEPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class MeleePlant : public Plant {
public:
    virtual void performAction(IGameplayMediator* mediator) override = 0;
};

#endif
