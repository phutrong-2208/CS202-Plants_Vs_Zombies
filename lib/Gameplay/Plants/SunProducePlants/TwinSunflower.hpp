#pragma once
#include "Gameplay/Plants/Plant.hpp"
class TwinSunflower : public Plant {
public:
    PlantType getType() override;
    void plantSetup() override;
};
