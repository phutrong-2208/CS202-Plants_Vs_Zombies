#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"

Repeater::Repeater() = default;

PlantType Repeater::getType() {
    return REPEATER;
}

Vector2 Repeater::getProjectileSpawnPosition() {
    return {bounds.x + 108.0f, bounds.y + 45.0f}; // Using same offset as Peashooter for now
}
