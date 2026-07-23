#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Common.hpp"

class Projectile{
private:
    int damage = 0;
    float radius = 0.0f;
    float range = 0.0f;
    float totalDistance = 0.0f; //total distance the projectile travelled
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};
    Texture2D *texture = nullptr;

    bool despawned = false;
public:
    Projectile() = default;
    Projectile(Vector2 pos, Vector2 vel, int _damage, float _radius, float _range);
    virtual ~Projectile() = default;

    void setTexture(Texture2D *newTexture);
    void setDamage(int _d);
    void setPosition(Vector2 newPos);
    void setVelocity(Vector2 newVel);


    int getDamage(void) const;
    float getRadius(void) const;
    float getRange(void) const;
    Vector2 getPosition(void) const;
    Vector2 getVelocity(void) const;

    bool isDespawned(void) const;
    void Despawn(void);
    
    virtual void update(float dt);
    virtual void draw(void) const;
};

#endif
