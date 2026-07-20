#ifndef PLANT_HPP
#define PLANT_HPP

#include <Core/TextureManager.hpp>
#include <Core/AnimationManager.hpp>
#include <Gameplay/Animation/ReanimInstance.hpp>

class Plant {
protected:
    int health;
    int sunCost;
    ReanimInstance animation;
public:
    Plant(int hp, int cost);
    void receiveDamage(int damage);

    void updateTime(float deltaSeconds);
    void setReanimInstance(ReanimInstance anim);
    void draw(Rectangle hitbox);
    
    bool isDead() const;
    int getHealth() const;
    int getCost() const;
};

enum PlantType : int {
    PEASHOOTER,
    SUNFLOWER,
    REPEATER,
    SNOWPEA,
    WALLNUT,
    CACTUS,
    PLANT_COUNT
};

class PlantFactory {
private:
    TextureManager* textureManager;
    AnimationManager* animationManager;
public:
    void setTextureManager(TextureManager* manager);
    void setAnimationManager(AnimationManager* manager);

    ReanimInstance createReanim(float scalar, const std::string& packageName, const std::string& animName, const std::string& clipLoopName);
    std::unique_ptr <Plant> createPlant(PlantType pType);
};

#endif
