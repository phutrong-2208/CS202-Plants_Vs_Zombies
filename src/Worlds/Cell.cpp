#include "Worlds/Cell.hpp"
#include "Gameplay/Plants/DefensivePlants/Pumpkin.hpp"

Cell::Cell() {}
Cell::~Cell() {}

bool Cell::isOccupied() const {
    return plant != nullptr || pumpkin != nullptr;
}
bool Cell::hasNormalPlant() const {
    return plant != nullptr;
}

void Cell::updateTime(float deltaSeconds) {
    if (plant) plant->updateTime(deltaSeconds);
    if (pumpkin) pumpkin->updateTime(deltaSeconds);
    checkDeadPlants();
}

void Cell::draw(Rectangle hitbox) {
    Pumpkin* p = dynamic_cast<Pumpkin*>(pumpkin.get());
    if (p) {
        p->drawBack(hitbox);
    }
    if (plant) {
        plant->draw(hitbox);
    }
    if (p) {
        p->drawFront(hitbox);
    } else if (pumpkin) {
        pumpkin->draw(hitbox);
    }
}

bool Cell::placePlant(std::unique_ptr<Plant> newPlant) {
    if (!newPlant) return false;
    PlantType type = newPlant->getType();
    
    if (type == PUMPKIN) {
        if (pumpkin) return false; // Already has pumpkin
        pumpkin = std::move(newPlant);
        return true;
    }

    if (plant) {
        PlantType existingType = plant->getType();

        // Check if newPlant is a valid upgrade for the existing plant
        bool isUpgrade = (type == GATLINGPEA    && existingType == REPEATER) ||
                         (type == TWINSUNFLOWER && existingType == SUNFLOWER) ||
                         (type == GLOOMSHROOM   && existingType == FUMESHROOM) ||
                         (type == SPIKEROCK     && existingType == CALTROP) ||
                         (type == GOLDMAGNET    && existingType == MAGNETSHROOM) ||
                         (type == WINTERMELON   && existingType == MELONPULT) ||
                         (type == COBCANNON     && existingType == CORNPULT) ||
                         (type == CATTAIL       && (existingType == LILYPAD || existingType == SEASHROOM)) ||
                         (type == COFFEEBEAN);

        if (isUpgrade) {
            if (type != COFFEEBEAN) {
                plant = std::move(newPlant); // Upgrade replaces the base plant
            }
            return true;
        }

        return false; // Cell already has a plant and cannot be upgraded
    }

    // Empty cell: place plant
    plant = std::move(newPlant);
    return true;
}

Plant* Cell::getPlant() const { return plant.get(); }
Plant* Cell::getPumpkin() const { return pumpkin.get(); }

void Cell::checkDeadPlants() {
    if (pumpkin && pumpkin->isDead()) pumpkin.reset();
    if (plant && plant->isDead()) plant.reset();
}

void Cell::removePlant() {
    pumpkin.reset();
    plant.reset();
}
