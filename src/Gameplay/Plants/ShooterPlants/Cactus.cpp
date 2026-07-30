#include "Gameplay/Plants/ShooterPlants/Cactus.hpp"

Cactus::Cactus() = default;

PlantType Cactus::getType() {
    return CACTUS;
}

Vector2 Cactus::getProjectileSpawnPosition() {
    return {bounds.x + 108.0f, bounds.y + 45.0f}; // Using same offset as Peashooter for now
}
