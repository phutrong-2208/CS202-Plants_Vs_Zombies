#pragma once
#include "Gameplay/Zombies/Zombie.hpp"

class NewspaperZombie : public Zombie {
private:
    bool isAngry = false;

public:
    NewspaperZombie() = default;
    void onArmorBroken() override;
};
