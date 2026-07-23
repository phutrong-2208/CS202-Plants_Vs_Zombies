#ifndef PLANT_HPP
#define PLANT_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>
#include <Gameplay/Zombies/ZombieManager.hpp>

class ProjectileManager;

enum PlantType : int {
    PEASHOOTER,
    SUNFLOWER,
    REPEATER,
    SNOWPEA,
    WALLNUT,
    CACTUS,
    PLANT_COUNT
};

class PlantData {
private:
    float baseHealth = 100.0f;
    float baseDamage = 10.0f, buffDamage = 20.0f;
    int sunCost = 100;
    float projectileCooldown = 0.0f;
    float projectileRange = 0.0f;

    // Reanim metadata
    float reanimScalar = 1.5f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;
public:
    PlantData() = default;
    PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float projectileCooldown, float projectileRange);
      
    float getBaseHealth() const;
    float getDamage(bool buffed) const;
    int getSunCost() const;
    float getProjectileCooldown() const;
    float getProjectileRange() const;

    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;

    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& anim);
    void setReanimClip(const std::string& clip);
};

class Plant {
protected:
    int health = 0;
    float cooldownTimer = 0.0f;
    PlantData* plantData = nullptr;
    ReanimInstance animation;
    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    void plantSetup();
public:
    Plant() = default;
    virtual ~Plant() = default;
    void receiveDamage(int damage);

    virtual void attack(
        ProjectileManager& projectileManager,
        const ZombieManager& zombieManager
    ) {}
    virtual bool hasTarget(const ZombieManager& zombieManager) const;

    void updateTime(float deltaSeconds);
    void setBounds(Rectangle newBounds);
    Rectangle getBounds() const;
    void setReanimInstance(ReanimInstance anim);
    void setPlantData(PlantData* pData);
    void draw(Rectangle hitbox);
    
    bool isDead() const;
    int getHealth() const;
    int getCost() const;
};

#endif
