#pragma once
#include "Gameplay/Plants/Plant.hpp"
class SunShroom : public Plant {
public:
    PlantType getType() override;
    void plantSetup() override;
};
