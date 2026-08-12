#pragma once
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlant.hpp"
class CherryBomb : public ExplosivePlant {
public:
    PlantType getType() override;
};
