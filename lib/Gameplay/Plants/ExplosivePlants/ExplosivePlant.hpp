#ifndef EXPLOSIVEPLANT_HPP
#define EXPLOSIVEPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class ExplosivePlant : public Plant {
public:
    ExplosivePlant();
    virtual ~ExplosivePlant() = default;

    virtual void performAction(IGameplayMediator* mediator) override;
};

#endif
