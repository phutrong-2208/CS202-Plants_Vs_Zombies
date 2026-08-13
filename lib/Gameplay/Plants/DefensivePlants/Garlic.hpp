#ifndef GARLIC_HPP
#define GARLIC_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class Garlic : public DefensivePlant {
public:
    PlantType getType() override;
    void receiveDamage(int damage, Zombie* attacker = nullptr, IGameplayMediator* mediator = nullptr) override;
};

#endif
