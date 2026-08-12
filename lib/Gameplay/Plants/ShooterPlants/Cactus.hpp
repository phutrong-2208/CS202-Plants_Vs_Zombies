#ifndef CACTUS_HPP
#define CACTUS_HPP

#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

class Cactus : public ShooterPlant {
public:
    Cactus();
    PlantType getType() override;
};

#endif
