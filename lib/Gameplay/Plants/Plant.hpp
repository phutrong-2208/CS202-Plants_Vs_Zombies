#ifndef PLANT_HPP
#define PLANT_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>

enum PlantType : int {
    PEASHOOTER,
    SUNFLOWER,
    CHERRYBOMB,
    WALLNUT,
    POTATOMINE,
    SNOWPEA,
    CHOMPER,
    REPEATER,
    PUFFSHROOM,
    SUNSHROOM,
    FUMESHROOM,
    GRAVEBUSTER,
    HYPNOSHROOM,
    SCAREDYSHROOM,
    ICESHROOM,
    DOOMSHROOM,
    LILYPAD,
    SQUASH,
    THREEPEATER,
    TANGLEKELP,
    JALAPENO,
    CALTROP,
    TORCHWOOD,
    TALLNUT,
    SEASHROOM,
    PLANTERN,
    CACTUS,
    BLOVER,
    SPLITPEA,
    STARFRUIT,
    PUMPKIN,
    MAGNETSHROOM,
    CABBAGEPULT,
    CORNPULT,
    COFFEEBEAN,
    GARLIC,
    UMBRELLALEAF,
    MARIGOLD,
    MELONPULT,
    GATLINGPEA,
    TWINSUNFLOWER,
    GLOOMSHROOM,
    CATTAIL,
    SPIKEROCK,
    GOLDMAGNET,
    WINTERMELON,
    COBCANNON,
    IMITATER,
    PLANT_COUNT
};

const char* getPlantTextureKey(PlantType type);
const std :: vector<PlantType>& getAllPlantTypes();

class PlantData {
private:
    float baseHealth = 100.0f;
    float baseDamage = 10.0f, buffDamage = 20.0f;
    int sunCost = 100;
    float projectileCooldown = 0.0f;

    // Reanim metadata
    float reanimScalar = 1.5f;
    std::string reanimPackage;
    std::string reanimAnim;
    std::string reanimClip;
public:
    PlantData() = default;
    PlantData(float baseHealth, float baseDamage, float buffDamage, int sunCost, float projectileCooldown);
      
    float getBaseHealth() const;
    float getDamage(bool buffed) const;
    int getSunCost() const;
    float getProjectileCooldown() const;

    float getReanimScalar() const;
    const std::string& getReanimPackage() const;
    const std::string& getReanimAnim() const;
    const std::string& getReanimClip() const;

    void setReanimScalar(float scalar);
    void setReanimPackage(const std::string& package);
    void setReanimAnim(const std::string& anim);
    void setReanimClip(const std::string& clip);

    void setBaseHealth(float health);
    void setBaseDamage(float damage);
    void setBuffDamage(float damage);
    void setSunCost(int cost);
    void setProjectileCooldown(float cooldown);
};

class Plant {
protected:
    float health = 0;
    float cooldownTimer = 0.0f;
    bool buffed = false;

    PlantData* plantData = nullptr;
    ReanimInstance animation;
    Rectangle bounds = {0.0f, 0.0f, 0.0f, 0.0f};

    void plantSetup();
public:
    Plant() = default;
    virtual ~Plant() = default;
    void receiveDamage(int damage);

    void updateTime(float deltaSeconds);
    void resetCooldown();

    void setBounds(Rectangle newBounds);
    Rectangle getBounds() const;

    virtual Vector2 getProjectileSpawnPosition();
    virtual PlantType getType();
    
    void setReanimInstance(ReanimInstance anim);
    void setPlantData(PlantData* pData);
    void draw(Rectangle hitbox);
    
    bool isDead() const;
    bool isOnCooldown() const;
    float getHealth() const;
    int getCost() const;
    float getDamage() const;
};

#endif
