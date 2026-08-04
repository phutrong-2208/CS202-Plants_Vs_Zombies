#ifndef SUNFLOWER_HPP
#define SUNFLOWER_HPP

#include "Gameplay/Plants/Plant.hpp"

class Sunflower : public Plant {
public:
    Sunflower();
    PlantType getType() override;
    void resetCooldown();
};

#endif
