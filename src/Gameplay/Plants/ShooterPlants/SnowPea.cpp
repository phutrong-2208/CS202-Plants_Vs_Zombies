#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"

SnowPea::SnowPea() = default;

PlantType SnowPea::getType() {
    return SNOWPEA;
}

Vector2 SnowPea::getProjectileSpawnPosition() {
    return {bounds.x + 108.0f, bounds.y + 45.0f}; // Using same offset as Peashooter for now
}
