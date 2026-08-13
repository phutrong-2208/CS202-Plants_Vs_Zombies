#ifndef HYPNOSHROOM_HPP
#define HYPNOSHROOM_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class HypnoShroom : public DefensivePlant {
public:
    PlantType getType() override;
    void receiveDamage(int damage, Zombie* attacker = nullptr, IGameplayMediator* mediator = nullptr) override;
};

#endif
