#include "Gameplay/Zombies/ZombieFactory.hpp"
#include "Gameplay/Zombies/NormalZombie.hpp"

namespace {
    const std::map<std::string, ZombieType> zombieTypeNameMap = {
        {"NORMAL_ZOMBIE", NORMAL_ZOMBIE}
    };
}

ZombieDataset::ZombieDataset() {
    loadFromFile(std::string(PROJECT_DIR) + "assets/data/stats/ZombieConfig.txt");
}

void ZombieDataset::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if(!file.is_open()) {
        std::cerr << "[ZombieDataset] Failed to open: " << filepath << '\n';
        return;
    }

    ZombieType currentType = ZOMBIE_COUNT;
    std::unique_ptr<ZombieData> currentData;
    std::string line;

    while(std::getline(file, line)) {
        const size_t start = line.find_first_not_of(" \t\r\n");
        if(start == std::string::npos) continue;
        line = line.substr(start);

        if(line.front() == '[' && line.back() == ']') {
            if(currentData && currentType != ZOMBIE_COUNT) {
                zombieDatasetMap[currentType] = std::move(currentData);
            }

            const std::string name = line.substr(1, line.size() - 2);
            const auto it = zombieTypeNameMap.find(name);
            if(it == zombieTypeNameMap.end()) {
                currentType = ZOMBIE_COUNT;
                currentData.reset();
                continue;
            }

            currentType = it->second;
            currentData = std::make_unique<ZombieData>();
            continue;
        }

        if(!currentData) continue;
        const size_t equals = line.find('=');
        if(equals == std::string::npos) continue;

        const std::string key = line.substr(0, equals);
        const std::string value = line.substr(equals + 1);

        if(key == "HEALTH") currentData->setBaseHealth(std::stoi(value));
        else if(key == "SPEED") currentData->setMoveSpeed(std::stof(value));
        else if(key == "ATTACK_DAMAGE") currentData->setAttackDamage(std::stoi(value));
        else if(key == "REANIM_SCALAR") currentData->setReanimScalar(std::stof(value));
        else if(key == "REANIM_PACKAGE") currentData->setReanimPackage(value);
        else if(key == "REANIM_ANIM") currentData->setReanimAnim(value);
        else if(key == "REANIM_CLIP") currentData->setReanimClip(value);
    }

    if(currentData && currentType != ZOMBIE_COUNT) {
        zombieDatasetMap[currentType] = std::move(currentData);
    }
}

ZombieData* ZombieDataset::getZombieData(ZombieType type) {
    const auto it = zombieDatasetMap.find(type);
    return it == zombieDatasetMap.end() ? nullptr : it->second.get();
}

void ZombieFactory::setTextureManager(TextureManager* manager) {
    textureManager = manager;
}

void ZombieFactory::setAnimationManager(AnimationManager* manager) {
    animationManager = manager;
}

void ZombieFactory::loadZombieMechanics() {
    zombieDataStorage = std::make_unique<ZombieDataset>();
    creatorMap[NORMAL_ZOMBIE] = []() {
        return std::make_unique<NormalZombie>();
    };
}

ReanimInstance ZombieFactory::createReanim(
    float scalar,
    const std::string& packageName,
    const std::string& animName,
    const std::string& clipLoopName
) {
    ReanimInstance reanim;
    if(!textureManager || !animationManager) return reanim;

    reanim.setTextureScalar(scalar);
    reanim.setTexturePackage(textureManager->getPackage(packageName));
    reanim.setAnimation(animationManager->getAnimationData(animName));
    reanim.playClip(clipLoopName);
    return reanim;
}

std::unique_ptr<Zombie> ZombieFactory::createZombie(
    ZombieType type,
    Rectangle hitbox
) {
    const auto creator = creatorMap.find(type);
    if(creator == creatorMap.end() || !zombieDataStorage) return nullptr;

    ZombieData* data = zombieDataStorage->getZombieData(type);
    if(!data) return nullptr;

    std::unique_ptr<Zombie> zombie = creator->second();
    zombie->setZombieData(data);
    zombie->setHitbox(hitbox);
    zombie->setReanimInstance(
        createReanim(
            data->getReanimScalar(),
            data->getReanimPackage(),
            data->getReanimAnim(),
            data->getReanimClip()
        )
    );
    return zombie;
}
