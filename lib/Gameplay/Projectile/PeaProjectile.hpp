#ifndef PEA_PROJECTILE_HPP
#define PEA_PROJECTILE_HPP

#include "Gameplay/Projectile/Projectile.hpp"

class PeaProjectile : public Projectile {
public:
    PeaProjectile(Vector2 position, Vector2 velocity, int damage,
                  float radius, float range, Texture2D* texture = nullptr);
};

#endif
