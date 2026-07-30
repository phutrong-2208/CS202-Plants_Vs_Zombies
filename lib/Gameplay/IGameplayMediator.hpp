#ifndef IGAMEPLAYMEDIATOR_HPP
#define IGAMEPLAYMEDIATOR_HPP

#include <Gameplay/Plants/Plant.hpp>
#include <Gameplay/Projectile/Projectile.hpp>
#include <Gameplay/Zombies/Zombie.hpp>

class IGameplayMediator {
public:
    virtual void addProjectile(ProjectileType projType, Vector2 position, float damage) = 0;
    // virtual bool hasTarget(ProjectTileType projType, Vector2 position) = 0;
};
#endif