#pragma once
#include "Gameplay/Plants/DefensivePlants/DefensivePlant.hpp"

class Pumpkin : public DefensivePlant {
public:
    PlantType getType() override;
    Rectangle getHitbox() const override;
    void draw(Rectangle hitbox) override;
    void drawBack(Rectangle hitbox);
    void drawFront(Rectangle hitbox);
};
