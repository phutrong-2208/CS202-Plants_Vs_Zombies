#ifndef SNOWPEA_HPP
#define SNOWPEA_HPP

#include "Gameplay/Plants/Plant.hpp"

class SnowPea : public Plant {
public:
    SnowPea();
    PlantType getType() override;
    Vector2 getProjectileSpawnPosition() override;
};

#endif
