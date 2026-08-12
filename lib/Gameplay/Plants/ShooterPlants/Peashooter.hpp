#ifndef PEASHOOTER_HPP
#define PEASHOOTER_HPP

#include "Gameplay/Plants/ShooterPlants/ShooterPlant.hpp"

class Peashooter : public ShooterPlant {
private:

public:
    Peashooter();
    
    PlantType getType() override;
};

#endif
