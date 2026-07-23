#ifndef ZOMBIE_FACTORY_HPP
#define ZOMBIE_FACTORY_HPP

#include <Gameplay/Zombies/Zombie.hpp>
#include <functional>

class ZombieDataset {
private:
    std::map<ZombieType, std::unique_ptr<ZombieData>> zombieDatasetMap;
    void loadFromFile(const std::string& filepath);

public:
    ZombieDataset();
    ZombieData* getZombieData(ZombieType type);
};

class ZombieFactory {
private:
    TextureManager* textureManager = nullptr;
    AnimationManager* animationManager = nullptr;
    std::unique_ptr<ZombieDataset> zombieDataStorage = nullptr;

    using ZombieCreator = std::function<std::unique_ptr<Zombie>()>;
    std::map<ZombieType, ZombieCreator> creatorMap;

    ReanimInstance createReanim(
        float scalar,
        const std::string& packageName,
        const std::string& animName,
        const std::string& clipLoopName
    );

public:
    void setTextureManager(TextureManager* manager);
    void setAnimationManager(AnimationManager* manager);
    void loadZombieMechanics();

    std::unique_ptr<Zombie> createZombie(
        ZombieType type,
        Rectangle hitbox
    );
};

#endif
