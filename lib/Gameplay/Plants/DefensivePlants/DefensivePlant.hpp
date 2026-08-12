#ifndef DEFENSIVEPLANT_HPP
#define DEFENSIVEPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class DefensivePlant : public Plant {
public:
    DefensivePlant();
    virtual ~DefensivePlant() = default;

    virtual void performAction(IGameplayMediator* mediator) override;
};

#endif
