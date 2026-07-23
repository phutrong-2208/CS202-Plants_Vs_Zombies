#ifndef CACTUS_PROJECTILE_HPP
#define CACTUS_PROJECTILE_HPP

#include "Gameplay/Projectile/Projectile.hpp"

class CactusProjectile : public Projectile {
public:
    CactusProjectile(Vector2 position, Vector2 velocity, int damage,
                     float radius, float range, Texture2D* texture = nullptr);
};

#endif
