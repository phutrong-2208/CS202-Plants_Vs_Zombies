#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Common.hpp"

enum ProjectileType : int {
    PROJECTILE_PEA,
    PROJECTILE_SNOWPEA,
    PROJECTILE_CACTUS,
    PROJECTILE_STAR,
    PROJECTILE_COUNT
};

class ProjectileData {
private:
    float radius = 0.0f;
    float range = 0.0f;
    Vector2 velocity = {0.0f, 0.0f};
    std::string textureName = "";
public:
    void setRadius(float radius);
    void setRange(float range);
    void setVelocity(Vector2 velocity);
    void setTextureName(const std::string& textureName);    

    float getRadius() const;
    float getRange() const;
    Vector2 getVelocity() const;
    const std::string& getTextureName() const;
};

class Projectile {
private:
    ProjectileData* projData = nullptr;
    float damage = 0;
    float totalDistance = 0.0f; //total distance the projectile travelled
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};
    Texture2D *texture = nullptr;

    bool despawned = false;
public:
    Projectile() = default;
    // Projectile(Vector2 pos, Vector2 vel, float _damage, float _radius, float _range, Texture2D* tex);

    // Data-driven constructor: velocity/radius from ProjectileData, damage and range from caller
    Projectile(Vector2 pos, ProjectileData* projData, float damage, float range, Texture2D* tex);

    virtual ~Projectile() = default;

    void setTexture(Texture2D *newTexture);
    void setDamage(float _d);
    void setPosition(Vector2 newPos);
    void setVelocity(Vector2 newVel);


    float getDamage(void) const;
    float getRadius(void) const;
    float getRange(void) const;
    Vector2 getPosition(void) const;
    Rectangle getHitbox(void) const;
    Vector2 getVelocity(void) const;

    bool isDespawned(void) const;
    void Despawn(void);
    
    virtual void update(float dt);
    virtual void draw(void) const;
};

#endif
