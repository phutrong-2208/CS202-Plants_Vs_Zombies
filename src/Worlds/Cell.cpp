#include "Worlds/Cell.hpp"

Cell::Cell() {}
Cell::~Cell() {}

bool Cell::isOccupied() const {
    return basePlant != nullptr || plant != nullptr || pumpkin != nullptr;
}
bool Cell::hasNormalPlant() const {
    return plant != nullptr;
}

void Cell::updateTime(float deltaSeconds) {
    if (basePlant) basePlant->updateTime(deltaSeconds);
    if (plant) plant->updateTime(deltaSeconds);
    if (pumpkin) pumpkin->updateTime(deltaSeconds);
    checkDeadPlants();
}

void Cell::draw(Rectangle hitbox) {
    if (basePlant) basePlant->draw(hitbox);
    if (plant) plant->draw(hitbox);
    if (pumpkin) pumpkin->draw(hitbox);
}

bool Cell::placePlant(std::unique_ptr<Plant> newPlant) {
    if (!newPlant) return false;
    PlantType type = newPlant->getType();
    
    if (type == LILYPAD) {
        if (basePlant) return false; // Already has lilypad
        basePlant = std::move(newPlant);
        return true;
    } else if (type == PUMPKIN) {
        if (pumpkin) return false; // Already has pumpkin
        pumpkin = std::move(newPlant);
        return true;
    } else {
        if (plant) return false; // Already has a normal plant
        plant = std::move(newPlant);
        return true;
    }
}

Plant* Cell::getPlant() const { return plant.get(); }
Plant* Cell::getPumpkin() const { return pumpkin.get(); }
Plant* Cell::getBasePlant() const { return basePlant.get(); }

void Cell::checkDeadPlants() {
    if (pumpkin && pumpkin->isDead()) pumpkin.reset();
    if (plant && plant->isDead()) plant.reset();
    if (basePlant && basePlant->isDead()) basePlant.reset();
}

void Cell::removePlant() {
    pumpkin.reset();
    plant.reset();
    basePlant.reset();
}
