#include "Gameplay/Projectile/StarProjectile.hpp"

StarProjectile :: StarProjectile(Vector2 position, Vector2 velocity, int damage,
                                 float radius, float range, Texture2D* texture)
    : Projectile(position, velocity, damage, radius, range) {
    setTexture(texture);
}
