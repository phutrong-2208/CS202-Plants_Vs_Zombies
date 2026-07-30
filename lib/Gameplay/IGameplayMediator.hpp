#ifndef IGAMEPLAYMEDIATOR_HPP
#define IGAMEPLAYMEDIATOR_HPP

#include <Gameplay/Plants/Plant.hpp>
#include <Gameplay/Projectile/Projectile.hpp>
#include <Gameplay/Zombies/Zombie.hpp>

static const std::map <PlantType, ProjectileType> projectileConvert = {
    {PEASHOOTER, PROJECTILE_PEA}
};

class IGameplayMediator {
public:
    virtual ~IGameplayMediator() = default;
    virtual void addProjectile(PlantType plantType, Vector2 position, float damage) = 0;
    virtual bool hasTarget(PlantType plantType, Vector2 spawnPos, Rectangle bounds) = 0;
};

#endif