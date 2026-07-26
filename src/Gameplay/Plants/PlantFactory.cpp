#include <Gameplay/Plants/PlantFactory.hpp>


//Shooter Plants
#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"
#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"
#include "Gameplay/Plants/ShooterPlants/Cactus.hpp"


// Maps section name strings from PlantConfig.txt to PlantType enum values
static const std::map<std::string, PlantType> plantTypeNameMap = {
    {"PEASHOOTER", PEASHOOTER},
    {"SUNFLOWER",  SUNFLOWER},
    {"REPEATER",   REPEATER},
    {"SNOWPEA",    SNOWPEA},
    {"WALLNUT",    WALLNUT},
    {"CACTUS",     CACTUS}
};


///////////////////////////
///     PLANT DATASET   ///
///////////////////////////

PlantDataset::PlantDataset() {
    std::string configPath = std::string(PROJECT_DIR) + "assets/data/config/PlantConfig.txt";
    loadFromFile(configPath);
}

void PlantDataset::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[PlantDataset] Failed to open: " << filepath << std::endl;
        return;
    }

    std::string line;
    PlantType currentType = PLANT_COUNT;
    std::unique_ptr<PlantData> currentData = nullptr;

    while (std::getline(file, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        // Skip empty lines
        if (line.empty()) continue;

        // Section header: [PLANT_NAME]
        if (line.front() == '[' && line.back() == ']') {
            // Save previous plant data if any
            if (currentData && currentType != PLANT_COUNT) {
                plantDatasetMap[currentType] = std::move(currentData);
            }

            std::string sectionName = line.substr(1, line.size() - 2);
            auto it = plantTypeNameMap.find(sectionName);
            if (it != plantTypeNameMap.end()) {
                currentType = it->second;
                currentData = std::make_unique<PlantData>();
            } else {
                std::cerr << "[PlantDataset] Unknown plant type: " << sectionName << std::endl;
                currentType = PLANT_COUNT;
                currentData = nullptr;
            }
            continue;
        }

        // Key=Value pair
        if (!currentData) continue;

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos) continue;

        std::string key = line.substr(0, eqPos);
        std::string value = line.substr(eqPos + 1);

        if (key == "HEALTH") {
            currentData->setBaseHealth(std::stof(value));
        } else if (key == "SUN_COST") {
            currentData->setSunCost(std::stoi(value));
        } else if (key == "BASE_DAMAGE") {
            currentData->setBaseDamage(std::stof(value));
        } else if (key == "BUFF_DAMAGE") {
            currentData->setBuffDamage(std::stof(value));
        } else if (key == "COOLDOWN") {
            currentData->setProjectileCooldown(std::stof(value));
        } else if (key == "RANGE") {
            currentData->setProjectileRange(std::stof(value));
        } else if (key == "REANIM_SCALAR") {
            currentData->setReanimScalar(std::stof(value));
        } else if (key == "REANIM_PACKAGE") {
            currentData->setReanimPackage(value);
        } else if (key == "REANIM_ANIM") {
            currentData->setReanimAnim(value);
        } else if (key == "REANIM_CLIP") {
            currentData->setReanimClip(value);
        }
    }

    // Don't forget the last plant entry
    if (currentData && currentType != PLANT_COUNT) {
        plantDatasetMap[currentType] = std::move(currentData);
    }

    file.close();
}

PlantData* PlantDataset::getPlantData(PlantType pType) {
    auto it = plantDatasetMap.find(pType);
    if (it != plantDatasetMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

///////////////////////////
///     PLANT FACTORY   ///
///////////////////////////

void PlantFactory::setTextureManager(TextureManager* manager) {
    textureManager = manager;
}
void PlantFactory::setAnimationManager(AnimationManager* manager) {
    animationManager = manager;
}

void PlantFactory::loadPlantMechanics() {
    plantDataStorage = std::make_unique <PlantDataset> ();

    // Register plant creators — one lambda per concrete plant subclass
    creatorMap[PEASHOOTER] = []() { return std::make_unique<Peashooter>(); };
    creatorMap[SUNFLOWER]  = []() { return std::make_unique<Sunflower>(); };
    creatorMap[REPEATER]   = []() { return std::make_unique<Repeater>(); };
    creatorMap[SNOWPEA]    = []() { return std::make_unique<SnowPea>(); };
    creatorMap[WALLNUT]    = []() { return std::make_unique<WallNut>(); };
    creatorMap[CACTUS]     = []() { return std::make_unique<Cactus>(); };
} 

ReanimInstance PlantFactory::createReanim(float scalar, const std::string& packageName, const std::string& animName, const std::string& clipLoopName) {
    ReanimInstance plantReanim;
    plantReanim.setTextureScalar(scalar);
    plantReanim.setTexturePackage(textureManager -> getPackage(packageName));
    plantReanim.setAnimation(animationManager -> getAnimationData(animName));
    plantReanim.playClip(clipLoopName); 

    return plantReanim;
}

std::unique_ptr <Plant> PlantFactory::createPlant(PlantType pType) {
    auto it = creatorMap.find(pType);
    if (it == creatorMap.end()) return nullptr;

    std::unique_ptr<Plant> plantPtr = it->second();

    // Attach shared PlantData (stats blueprint)
    PlantData* data = plantDataStorage->getPlantData(pType);
    if (data) {
        plantPtr->setPlantData(data);

        // Set up animation from the data-driven reanim metadata
        plantPtr->setReanimInstance(
            createReanim(data->getReanimScalar(), data->getReanimPackage(),
                         data->getReanimAnim(), data->getReanimClip())
        );
    }

    ///THIS JUST FOR TESTING, WOULD BE DELETED WHEN THE STRUCTURE IS DONE
    if (pType == PEASHOOTER && textureManager) {
        auto* peashooter = static_cast<Peashooter*>(plantPtr.get());
        TexturePackage* projectilePackage =
            textureManager->getPackage("Projectile");

        if (projectilePackage) {
            peashooter->setProjectileTexture(
                projectilePackage->GetTexture("PROJECTILEPEA")
            );
        }
    }

    return plantPtr;
}
