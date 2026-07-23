#ifndef STAR_PROJECTILE_HPP
#define STAR_PROJECTILE_HPP

#include "Gameplay/Projectile/Projectile.hpp"

class StarProjectile : public Projectile {
public:
    StarProjectile(Vector2 position, Vector2 velocity, int damage,
                   float radius, float range, Texture2D* texture = nullptr);
};

#endif
