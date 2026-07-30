#ifndef CACTUS_HPP
#define CACTUS_HPP

#include "Gameplay/Plants/Plant.hpp"

class Cactus : public Plant {
public:
    Cactus();
    PlantType getType() override;
    Vector2 getProjectileSpawnPosition() override;
};

#endif
