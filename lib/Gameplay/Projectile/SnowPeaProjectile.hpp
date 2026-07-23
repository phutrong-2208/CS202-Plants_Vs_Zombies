#ifndef SNOW_PEA_PROJECTILE_HPP
#define SNOW_PEA_PROJECTILE_HPP

#include "Gameplay/Projectile/Projectile.hpp"

class SnowPeaProjectile : public Projectile {
public:
    SnowPeaProjectile(Vector2 position, Vector2 velocity, int damage,
                      float radius, float range, Texture2D* texture = nullptr);
};

#endif
