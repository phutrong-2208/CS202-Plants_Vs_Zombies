#include "Worlds/Cell.hpp"

Cell :: Cell() : plant(nullptr) {}
Cell::~Cell() {
    plant.reset(nullptr);
}
bool Cell :: isOccupied() const {
    return plant != nullptr;
}

// Rectangle Cell :: getBounds() const {
//     return bounds;
// }

// void Cell :: setBounds(Rectangle newBounds) {
//     bounds = newBounds;
// }

void Cell::updateTime(float deltaSeconds) {
    if (plant == nullptr) return;
    plant -> updateTime(deltaSeconds);
}
void Cell::draw(Rectangle hitbox) {
    if (plant == nullptr) return;
    plant -> draw(hitbox);
}

bool Cell :: placePlant(std::unique_ptr<Plant> newPlant) {
    if (plant) {
        return false;
    }
    plant = std :: move(newPlant);
    return true;
}

Plant* Cell :: getPlant() const {
    return plant.get();
}

void Cell :: removePlant() {
    plant.reset(nullptr);
}
