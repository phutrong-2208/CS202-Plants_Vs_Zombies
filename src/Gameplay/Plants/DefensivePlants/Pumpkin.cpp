#include "Gameplay/Plants/DefensivePlants/Pumpkin.hpp"

PlantType Pumpkin::getType() { return PUMPKIN; }

Rectangle Pumpkin::getHitbox() const {
    return {
        bounds.x + 3.0f,
        bounds.y + bounds.height * 0.35f,
        bounds.width - 6.0f,
        bounds.height * 0.65f
    };
}

void Pumpkin::drawBack(Rectangle hitbox) {
    Rectangle pBox = { hitbox.x + 6.0f, hitbox.y + 35.0f, hitbox.width, hitbox.height };
    animation.unhideTrack("Pumpkin_back");
    animation.hideTrack("Pumpkin_front");
    animation.draw(pBox);
}

void Pumpkin::drawFront(Rectangle hitbox) {
    Rectangle pBox = { hitbox.x + 6.0f, hitbox.y + 35.0f, hitbox.width, hitbox.height };
    animation.hideTrack("Pumpkin_back");
    animation.unhideTrack("Pumpkin_front");
    animation.draw(pBox);
    DrawRectangleLinesEx(getHitbox(), 2.0f, ORANGE);
}

void Pumpkin::draw(Rectangle hitbox) {
    drawBack(hitbox);
    drawFront(hitbox);
}
