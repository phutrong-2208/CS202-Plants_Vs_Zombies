#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Gameplay/Plants/Plant.hpp"

class Peashooter : public Plant {
private:

public:
    Peashooter();
    
    PlantType getType() override;
    Vector2 getProjectileSpawnPosition() override;
};

#endif
