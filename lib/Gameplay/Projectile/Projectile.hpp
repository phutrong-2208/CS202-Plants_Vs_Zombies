#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Common.hpp"

enum ProjectileType : int {
    PROJECTILE_PEA,
    PROJECTILE_SNOWPEA,
    PROJECTILE_CACTUS,
    PROJECTILE_STAR,
    PROJECTILE_PUFF,
    PROJECTILE_CABBAGE,
    PROJECTILE_CORN,
    PROJECTILE_MELON,
    PROJECTILE_WINTERMELON,
    PROJECTILE_COUNT
};

class ProjectileData {
private:
    float radius = 0.0f;
    float range = 0.0f;
    Vector2 velocity = {0.0f, 0.0f};
    std::string textureName = "";
    float chillDuration = 0.0f;
    bool lobbed = false;
    Vector2 splashArea = {0.0f, 0.0f};
    float scale = 1.0f;
public:
    void setRadius(float radius);
    void setRange(float range);
    void setVelocity(Vector2 velocity);
    void setTextureName(const std::string& textureName);    
    void setChillDuration(float duration);
    void setLobbed(bool lobbed);
    void setSplashArea(Vector2 splashArea);
    void setScale(float scale);

    float getRadius() const;
    float getRange() const;
    Vector2 getVelocity() const;
    const std::string& getTextureName() const;
    float getChillDuration() const;
    bool isLobbed() const;
    Vector2 getSplashArea() const;
    float getScale() const;
};

class Projectile {
private:
    ProjectileData* projData = nullptr;
    float damage = 0;
    float totalDistance = 0.0f; //total distance the projectile travelled
    Vector2 position = {0.0f, 0.0f};
    Vector2 velocity = {0.0f, 0.0f};
    Texture2D *texture = nullptr;
    ProjectileType type = PROJECTILE_PEA;

    bool despawned = false;

    // Lobbed mechanics
    Vector2 targetPos = {0.0f, 0.0f};
    bool impacted = false;
    float gravity = 1500.0f; // Adjust as needed
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
    void setType(ProjectileType t);
    void setTarget(Vector2 target);

    ProjectileType getType(void) const;


    float getDamage(void) const;
    float getRadius(void) const;
    float getRange(void) const;
    Vector2 getPosition(void) const;
    Rectangle getHitbox(void) const;
    Rectangle getCollisionHitbox(void) const;
    Vector2 getVelocity(void) const;
    float getChillDuration(void) const;
    
    bool isLobbed() const;
    Vector2 getSplashArea() const;
    bool hasImpacted() const;

    bool isDespawned(void) const;
    void Despawn(void);
    
    virtual void update(float dt);
    virtual void draw(void) const;
};

#endif
