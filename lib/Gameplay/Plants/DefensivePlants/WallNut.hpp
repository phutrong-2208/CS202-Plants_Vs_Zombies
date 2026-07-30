#ifndef WALLNUT_HPP
#define WALLNUT_HPP

#include "Gameplay/Plants/Plant.hpp"

class WallNut : public Plant {
public:
    WallNut();
    PlantType getType() override;
};

#endif
