#include "Gameplay/Levels/Level.hpp"

namespace {
    std :: string trim(const std :: string& value) {
        const size_t begin = value.find_first_not_of(" \t\r\n");
        if(begin == std :: string :: npos) return "";

        const size_t end = value.find_last_not_of(" \t\r\n");
        return value.substr(begin, end - begin + 1);
    }

    bool parseBool(const std :: string& value) {
        return value == "true" || value == "TRUE" || value == "1";
    }

    MapType mapTypeFromName(const std :: string& name) {
        if(name == "DAY") return MapType :: DAY;
        if(name == "NIGHT") return MapType :: NIGHT;
        if(name == "POOL") return MapType :: POOL;
        if(name == "FOG") return MapType :: FOG;
        if(name == "ROOF") return MapType :: ROOF;
        return MapType :: UNKNOWN;
    }

    PlantType plantTypeFromName(const std :: string& name) {
        for(PlantType type : getAllPlantTypes()) {
            if(name == getPlantTextureKey(type)) return type;
        }
        return PLANT_COUNT;
    }

    ZombieType zombieTypeFromName(const std :: string& name) {
        static const std :: unordered_map<std :: string, ZombieType> types = {
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

        const auto found = types.find(name);
        return found == types.end() ? ZOMBIE_COUNT : found -> second;
    }
}

Level :: Level(LevelID _id) : id(_id) {
    loadFromFile();
}

void Level :: loadFromFile() {
    const std :: string path =
        std :: string(PROJECT_DIR) + "assets/data/levels/" + id.getFileName();

    std :: ifstream file(path);
    if(!file.is_open()) {
        TraceLog(LOG_ERROR, "Level: cannot open %s", path.c_str());
        return;
    }

    lanes.clear();
    availablePlants.clear();
    preselectedPlants.clear();
    waves.clear();

    Section currentSection = Section :: NONE;
    WaveConfig* currentWave = nullptr;
    std :: string line;
    int lineNumber = 0;

    while(std :: getline(file, line)) {
        lineNumber++;
        line = trim(line);
        if(line.empty() || line.front() == '#') continue;

        if(line.front() == '[' && line.back() == ']') {
            const std :: string sectionName = line.substr(1, line.size() - 2);

            currentWave = nullptr;

            if(sectionName == "LEVEL") {
                currentSection = Section :: LEVEL;
            }
            else if(sectionName == "PLANTS") {
                currentSection = Section :: PLANTS;
            }
            else if(sectionName.rfind("WAVE_", 0) == 0) {
                currentSection = Section :: WAVE;
                waves.emplace_back();
                currentWave = &waves.back();
            }
            else {
                currentSection = Section :: NONE;
                TraceLog(
                    LOG_WARNING,
                    "Level: unknown section '%s' at line %d",
                    sectionName.c_str(),
                    lineNumber
                );
            }
            continue;
        }

        const size_t equalPosition = line.find('=');
        if(equalPosition == std :: string :: npos) continue;

        const std :: string key = trim(line.substr(0, equalPosition));
        const std :: string value = trim(line.substr(equalPosition + 1));

        try {
            if(currentSection == Section :: NONE) {
                if(key == "FORMAT_VERSION") formatVersion = std :: stoi(value);
            }
            else if(currentSection == Section :: LEVEL) {
                if(key == "WORLD" && std :: stoi(value) != id.world) {
                    TraceLog(LOG_WARNING, "Level: WORLD does not match its filename");
                }
                else if(key == "STAGE" && std :: stoi(value) != id.stage) {
                    TraceLog(LOG_WARNING, "Level: STAGE does not match its filename");
                }
                else if(key == "MAP_TYPE") {
                    mapType = mapTypeFromName(value);
                    if(mapType == MapType :: UNKNOWN) {
                        TraceLog(LOG_WARNING, "Level: unknown map type '%s'", value.c_str());
                    }
                }
                else if(key == "STARTING_SUN") startingSun = std :: stoi(value);
                else if(key == "SEED_SLOTS") seedSlots = std :: stoi(value);
                else if(key == "ALLOW_PLANT_CHOOSER") {
                    allowPlantChooser = parseBool(value);
                }
                else if(key == "USE_SOD_ROLL_INTRO") {
                    useSodRollIntro = parseBool(value);
                }
                else if(key == "FIRST_WAVE_DELAY") {
                    firstWaveDelay = std :: stof(value);
                }
                else if(key == "BETWEEN_WAVE_DELAY") {
                    betweenWaveDelay = std :: stof(value);
                }
                else if(key == "LANES") parseLanes(value);
            }
            else if(currentSection == Section :: PLANTS) {
                if(key == "AVAILABLE") parsePlants(value, availablePlants);
                else if(key == "PRESELECTED") parsePlants(value, preselectedPlants);
                else if(key == "REWARD") {
                    rewardPlant = plantTypeFromName(value);
                    if(rewardPlant == PLANT_COUNT) {
                        TraceLog(LOG_WARNING, "Level: unknown reward plant '%s'", value.c_str());
                    }
                }
            }
            else if(currentSection == Section :: WAVE && currentWave) {
                if(key == "ZOMBIES") parseZombies(value, *currentWave);
                else if(key == "SPAWN_INTERVAL_MIN") {
                    currentWave -> spawnIntervalMin = std :: stof(value);
                }
                else if(key == "SPAWN_INTERVAL_MAX") {
                    currentWave -> spawnIntervalMax = std :: stof(value);
                }
                else if(key == "IS_HUGE_WAVE") {
                    currentWave -> isHugeWave = parseBool(value);
                }
            }
        }
        catch(const std :: exception& exception) {
            TraceLog(
                LOG_WARNING, "Level: invalid value at line %d (%s)", lineNumber, exception.what()
            );
        }
    }

    for(WaveConfig& wave : waves) {
        if(wave.spawnIntervalMin < 0.0f) wave.spawnIntervalMin = 0.0f;
        if(wave.spawnIntervalMax < 0.0f) wave.spawnIntervalMax = 0.0f;

        if(wave.spawnIntervalMin > wave.spawnIntervalMax) {
            std :: swap(wave.spawnIntervalMin, wave.spawnIntervalMax);
        }
    }
}

void Level :: parseLanes(const std :: string& value) {
    lanes.clear();

    std :: string laneName;
    std :: stringstream stream(value);
    while(std :: getline(stream, laneName, ',')) {
        laneName = trim(laneName);

        if(laneName == "GRASS") lanes.push_back(LaneType :: GRASS);
        else if(laneName == "POOL") lanes.push_back(LaneType :: POOL);
        else if(laneName == "ROOF") lanes.push_back(LaneType :: ROOF);
        else if(laneName == "INACTIVE") lanes.push_back(LaneType :: INACTIVE);
        else TraceLog(LOG_WARNING, "Level: unknown lane type '%s'", laneName.c_str());
    }
}

void Level :: parsePlants(const std :: string& value, std :: vector<PlantType>& destination) {
    destination.clear();

    std :: string plantName;
    std :: stringstream stream(value);
    while(std :: getline(stream, plantName, ',')) {
        plantName = trim(plantName);
        const PlantType type = plantTypeFromName(plantName);

        if(type == PLANT_COUNT) {
            TraceLog(LOG_WARNING, "Level: unknown plant type '%s'", plantName.c_str());
            continue;
        }

        if(std :: find(destination.begin(), destination.end(), type) == destination.end()) {
            destination.push_back(type);
        }
    }
}

void Level :: parseZombies(const std :: string& value, WaveConfig& wave) {
    wave.zombies.clear();

    std :: string item;
    std :: stringstream stream(value);
    while(std :: getline(stream, item, ',')) {
        item = trim(item);

        const size_t colonPosition = item.find(':');
        if(colonPosition == std :: string :: npos) {
            TraceLog(LOG_WARNING, "Level: invalid zombie entry '%s'", item.c_str());
            continue;
        }

        const std :: string zombieName = trim(item.substr(0, colonPosition));
        const ZombieType type = zombieTypeFromName(zombieName);
        const int count = std :: stoi(trim(item.substr(colonPosition + 1)));

        if(type == ZOMBIE_COUNT) {
            TraceLog(LOG_WARNING, "Level: unknown zombie type '%s'", zombieName.c_str());
            continue;
        }
        if(count <= 0) {
            TraceLog(LOG_WARNING, "Level: zombie count must be positive");
            continue;
        }

        wave.zombies.push_back({type, count});
    }
}

const LevelID& Level :: getID() const {
    return id;
}

int Level :: getFormatVersion() const {
    return formatVersion;
}

MapType Level :: getMapType() const {
    return mapType;
}

int Level :: getStartingSun() const {
    return startingSun;
}

int Level :: getSeedSlots() const {
    return seedSlots;
}

bool Level :: allowsPlantChooser() const {
    return allowPlantChooser;
}

const std :: vector<LaneType>& Level :: getLanes() const {
    return lanes;
}

const std :: vector<PlantType>& Level :: getAvailablePlants() const {
    return availablePlants;
}

const std :: vector<PlantType>& Level :: getPreselectedPlants() const {
    return preselectedPlants;
}

PlantType Level :: getRewardPlant() const {
    return rewardPlant;
}

const WaveList& Level :: getWaves() const {
    return waves;
}

float Level :: getFirstWaveDelay() const {
    return firstWaveDelay;
}

float Level :: getBetweenWaveDelay() const {
    return betweenWaveDelay;
}

bool Level :: usesSodRollIntro() const {
    return useSodRollIntro;
}
