#ifndef PLANT_HPP
#define PLANT_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>

#include "PlantType.hpp"
#include <Gameplay/IGameplayMediator.hpp>

class PlantData {
private:
    float baseHealth = 100.0f;
    float baseDamage = 10.0f, buffDamage = 20.0f;
    int sunCost = 100;
    float projectileCooldown = 0.0f;
    float seedRecharge = 7.5f;

    // Reanim metadata
    float reanimScalar = 1.5f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;
    std::vector<std::string> reanimExtraClips;
public:
    PlantData() = default;
    PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float projectileCooldown);
      
    float getBaseHealth() const;
    float getDamage(bool buffed) const;
    int getSunCost() const;
    float getProjectileCooldown() const;
    float getSeedRecharge() const;

    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;
    const std::vector<std::string>& getReanimExtraClips() const;

    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& anim);
    void setReanimClip(const std::string& clip);
    void addReanimExtraClip(const std::string& clip);

    void setBaseHealth(float health);
    void setBaseDamage(float damage);
    void setBuffDamage(float damage);
    void setSunCost(int cost);
    void setProjectileCooldown(float cooldown);
    void setSeedRecharge(float recharge);
};

class Plant {
protected:
    float health = 0;
    float cooldownTimer = 0.0f;
    bool buffed = false;

    PlantData* plantData = nullptr;
    ReanimInstance animation;
    std::vector<ReanimInstance> extraAnimations;
    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    virtual void plantSetup();
public:
    Plant() = default;
    virtual ~Plant() = default;
    void receiveDamage(int damage);

    void triggerAnimation(const std::string& clipName);
    virtual void updateTime(float deltaSeconds);
    virtual void resetCooldown();
    virtual void performAction(IGameplayMediator* mediator);

    void setBounds(Rectangle newBounds);
    Rectangle getBounds() const;
    virtual Rectangle getHitbox() const;

    virtual Vector2 getProjectileSpawnPosition();
    virtual PlantType getType();
    
    void setReanimInstance(ReanimInstance anim);
    void addExtraReanimInstance(ReanimInstance anim);
    void setPlantData(PlantData* pData);
    virtual void draw(Rectangle hitbox);
    
    bool isDead() const;
    bool isOnCooldown() const;
    float getHealth() const;
    int getCost() const;
    float getDamage() const;
};

#endif
