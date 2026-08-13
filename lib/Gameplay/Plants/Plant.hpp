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
    float cooldown = 0.0f;
    float seedRecharge = 7.5f;

    // Reanim metadata
    float reanimScalar = 1.5f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;
    std::vector<std::pair<std::string, std::vector<std::string>>> clipLayers;
    std::vector<std::string> hiddenTracks;
    
    std::string actionAnim;
    std::vector<std::pair<std::string, std::vector<std::string>>> actionClipLayers;
public:
    PlantData() = default;
    PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float cooldown);
      
    float getBaseHealth() const;
    float getDamage(bool buffed) const;
    int getSunCost() const;
    float getCooldown() const;
    float getSeedRecharge() const;

    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;
    const std::vector<std::pair<std::string, std::vector<std::string>>>& getClipLayers() const;

    const std::string& getActionAnim() const;
    const std::vector<std::pair<std::string, std::vector<std::string>>>& getActionClipLayers() const;

    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& anim);
    void setReanimClip(const std::string& clip);
    void addClipLayer(const std::string& clip);
    void addClipLayerShowTrack(const std::string& track);
    void addHiddenTrack(const std::string& track);
    
    void setActionAnim(const std::string& anim);
    void addActionClipLayer(const std::string& clip);
    void addActionClipLayerShowTrack(const std::string& track);

    const std::vector<std::string>& getHiddenTracks() const;

    void setBaseHealth(float health);
    void setBaseDamage(float damage);
    void setBuffDamage(float damage);
    void setSunCost(int cost);
    void setCooldown(float cd);
    void setSeedRecharge(float recharge);
};

class Plant {
protected:
    float health = 0;
    float cooldownTimer = 0.0f;
    bool buffed = false;

    PlantData* plantData = nullptr;
    ReanimInstance animation;

    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    virtual void plantSetup();
public:
    Plant() = default;
    virtual ~Plant() = default;
    void receiveDamage(int damage);

    void triggerAnimation(const std::string& clipName);
    void triggerActionAnimation();
    virtual void updateTime(float deltaSeconds);
    virtual void onActionAnimationFinished() {}
    virtual void resetCooldown();
    virtual void performAction(IGameplayMediator* mediator);

    void setBounds(Rectangle newBounds);
    Rectangle getBounds() const;
    virtual Rectangle getHitbox() const;

    virtual Vector2 getProjectileSpawnPosition();
    virtual PlantType getType();
    
    void setReanimInstance(ReanimInstance anim);

    void setPlantData(PlantData* pData);
    virtual void draw(Rectangle hitbox);
    
    bool isDead() const;
    bool isOnCooldown() const;
    float getHealth() const;
    int getCost() const;
    float getDamage() const;
};

#endif
