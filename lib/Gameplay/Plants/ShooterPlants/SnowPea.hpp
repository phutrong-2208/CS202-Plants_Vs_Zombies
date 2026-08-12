#ifndef SNOWPEA_HPP
#define SNOWPEA_HPP

#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

class SnowPea : public ShooterPlant {
public:
    SnowPea();
    PlantType getType() override;
};

#endif
