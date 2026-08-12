#ifndef WALLNUT_HPP
#define WALLNUT_HPP

#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class WallNut : public DefensivePlant {
public:
    WallNut();
    PlantType getType() override;
};

#endif
