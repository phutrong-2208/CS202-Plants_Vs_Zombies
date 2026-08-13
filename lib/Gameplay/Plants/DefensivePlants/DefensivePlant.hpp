#ifndef DEFENSIVEPLANT_HPP
#define DEFENSIVEPLANT_HPP

#include "Gameplay/Plants/Plant.hpp"

class DefensivePlant : public Plant {
public:
    DefensivePlant();
    virtual ~DefensivePlant() = default;

    virtual void receiveDamage(int damage, Zombie* attacker = nullptr, IGameplayMediator* mediator = nullptr) override;
    virtual void updateTime(float deltaSeconds) override;
    virtual void performAction(IGameplayMediator* mediator) override;
};

#endif
