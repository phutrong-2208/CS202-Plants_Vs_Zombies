#include "Gameplay/Zombies/ZombieFactory.hpp"

namespace {
    const std::map<std::string, ZombieType> zombieTypeNameMap = {
        {"NORMAL_ZOMBIE", NORMAL_ZOMBIE},
        {"FLAG_ZOMBIE", FLAG_ZOMBIE},
        {"CONEHEAD_ZOMBIE", CONEHEAD_ZOMBIE},
        {"POLE_VAULTING_ZOMBIE", POLE_VAULTING_ZOMBIE},
        {"BUCKETHEAD_ZOMBIE", BUCKETHEAD_ZOMBIE},
        {"NEWSPAPER_ZOMBIE", NEWSPAPER_ZOMBIE},
        {"SCREEN_DOOR_ZOMBIE", SCREEN_DOOR_ZOMBIE},
        {"FOOTBALL_ZOMBIE", FOOTBALL_ZOMBIE},
        {"DANCING_ZOMBIE", DANCING_ZOMBIE},
        {"DANCER_ZOMBIE", DANCING_ZOMBIE},
        {"BACKUP_DANCER_ZOMBIE", BACKUP_DANCER_ZOMBIE},
        {"DUCKY_TUBE_ZOMBIE", DUCKY_TUBE_ZOMBIE},
        {"DUCKY_TUBE_CONEHEAD_ZOMBIE", DUCKY_TUBE_CONEHEAD_ZOMBIE},
        {"DUCKY_TUBE_BUCKETHEAD_ZOMBIE", DUCKY_TUBE_BUCKETHEAD_ZOMBIE},
        {"SNORKEL_ZOMBIE", SNORKEL_ZOMBIE},
        {"ZOMBONI_ZOMBIE", ZOMBONI_ZOMBIE},
        {"BOBSLED_TEAM_ZOMBIE", BOBSLED_TEAM_ZOMBIE},
        {"DOLPHIN_RIDER_ZOMBIE", DOLPHIN_RIDER_ZOMBIE},
        {"JACK_IN_THE_BOX_ZOMBIE", JACK_IN_THE_BOX_ZOMBIE},
        {"BALLOON_ZOMBIE", BALLOON_ZOMBIE},
        {"DIGGER_ZOMBIE", DIGGER_ZOMBIE},
        {"POGO_ZOMBIE", POGO_ZOMBIE},
        {"YETI_ZOMBIE", YETI_ZOMBIE},
        {"BUNGEE_ZOMBIE", BUNGEE_ZOMBIE},
        {"LADDER_ZOMBIE", LADDER_ZOMBIE},
        {"CATAPULT_ZOMBIE", CATAPULT_ZOMBIE},
        {"GARGANTUAR_ZOMBIE", GARGANTUAR_ZOMBIE},
        {"IMP_ZOMBIE", IMP_ZOMBIE},
        {"ZOMBOSS_ZOMBIE", ZOMBOSS_ZOMBIE}
    };

    std :: vector<std :: string> splitCommaSeparated(const std :: string& value) {
        std :: vector<std :: string> result;
        std :: string item;
        std :: stringstream stream(value);

        while(std :: getline(stream, item, ',')) {
            const size_t start = item.find_first_not_of(" \t");
            const size_t end = item.find_last_not_of(" \t\r");
            if(start != std :: string :: npos) {
                result.push_back(item.substr(start, end - start + 1));
            }
        }

        return result;
    }
}

ZombieDataset::ZombieDataset() {
    loadFromFile(std::string(PROJECT_DIR) + "assets/data/config/ZombieConfig.txt");
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
        else if(key == "ARMOR_HEALTH") currentData->setArmorHealth(std::stoi(value));
        else if(key == "ALTERNATE_HEALTH") currentData->setAlternateHealth(std::stoi(value));
        else if(key == "SPEED") currentData->setMoveSpeed(std::stof(value));
        else if(key == "ATTACK_DAMAGE") currentData->setAttackDamage(std::stoi(value));
        else if(key == "ATTACK_INTERVAL") currentData->setAttackInterval(std::stof(value));
        else if(key == "UNIT_COUNT") currentData->setUnitCount(std::stoi(value));
        else if(key == "REANIM_SCALAR") currentData->setReanimScalar(std::stof(value));
        else if(key == "REANIM_PACKAGE") currentData->setReanimPackage(value);
        else if(key == "REANIM_ANIM") currentData->setReanimAnim(value);
        else if(key == "REANIM_CLIP") currentData->setReanimClip(value);
        else if(key == "HIDDEN_TRACKS") currentData->setHiddenTracks(splitCommaSeparated(value));
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
    creatorMap.clear();
    for(int type = NORMAL_ZOMBIE; type < ZOMBIE_COUNT; ++type) {
        creatorMap[static_cast<ZombieType>(type)] = []() {
            return std::make_unique<Zombie>();
        };
    }
}

ReanimInstance ZombieFactory::createReanim(
    float scalar,
    const std::string& packageName,
    const std::string& animName,
    const std::string& clipLoopName,
    const std::vector<std::string>& hiddenTracks
) {
    ReanimInstance reanim;
    if(!textureManager || !animationManager) return reanim;

    reanim.setTextureScalar(scalar);
    reanim.setTexturePackage(textureManager->getPackage(packageName));
    reanim.setAnimation(animationManager->getAnimationData(animName));
    for(const std :: string& track : hiddenTracks) reanim.hideTrack(track);
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
            data->getReanimClip(),
            data->getHiddenTracks()
        )
    );
    return zombie;
}
