#include "Worlds/Cell.hpp"

Cell :: Cell() : plant(nullptr), bounds{-1, -1, -1, -1}{}

bool Cell :: isOccupied() const {
    return plant != nullptr;
}

Rectangle Cell :: getBounds() const {
    return bounds;
}

void Cell :: setBounds(Rectangle newBounds) {
    bounds = newBounds;
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
    plant.reset();
}
