#include <Gameplay/Plants/PlantFactory.hpp>


//Shooter Plants
#include "Gameplay/Plants/ShooterPlants/Peashooter.hpp"
#include "Gameplay/Plants/SunProducePlants/Sunflower.hpp"
#include "Gameplay/Plants/DefensivePlants/WallNut.hpp"
#include "Gameplay/Plants/ShooterPlants/SnowPea.hpp"
#include "Gameplay/Plants/ShooterPlants/Repeater.hpp"
#include "Gameplay/Plants/ShooterPlants/Cactus.hpp"
#include "Gameplay/Plants/ShooterPlants/ShooterPlantTypes.hpp"
#include "Gameplay/Plants/DefensivePlants/DefensivePlantTypes.hpp"
#include "Gameplay/Plants/ExplosivePlants/ExplosivePlantTypes.hpp"
#include "Gameplay/Plants/SunProducePlants/SunPlantTypes.hpp"
#include "Gameplay/Plants/MeleePlants/MeleePlantTypes.hpp"


// Maps section name strings from PlantConfig.txt to PlantType enum values
static const std::map<std::string, PlantType> plantTypeNameMap = [] {
    std::map<std::string, PlantType> result;
    for(PlantType type : getAllPlantTypes()) {
        result.emplace(getPlantTextureKey(type), type);
    }
    return result;
}();


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
            currentData->setCooldown(std::stof(value));
        } else if (key == "SEED_RECHARGE") {
            currentData->setSeedRecharge(std::stof(value));
        } else if (key == "REANIM_SCALAR") {
            currentData->setReanimScalar(std::stof(value));
        } else if (key == "REANIM_PACKAGE") {
            currentData->setReanimPackage(value);
        } else if (key == "REANIM_ANIM") {
            currentData->setReanimAnim(value);
        } else if (key == "REANIM_CLIP") {
            currentData->setReanimClip(value);
        } else if (key == "REANIM_CLIP_LAYER") {
            currentData->addClipLayer(value);
        } else if (key == "REANIM_HIDE_TRACKS" || key == "REANIM_CLIP_LAYER_SHOW_TRACKS" || key == "ACTION_LAYER_SHOW_TRACKS") {
            std::stringstream ss(value);
            std::string trackName;
            while (std::getline(ss, trackName, ',')) {
                // Trim spaces and carriage returns
                size_t start = trackName.find_first_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    trackName = trackName.substr(start);
                    size_t end = trackName.find_last_not_of(" \t\r\n");
                    if (end != std::string::npos) trackName = trackName.substr(0, end + 1);
                    
                    if (key == "REANIM_HIDE_TRACKS") currentData->addHiddenTrack(trackName);
                    else if (key == "REANIM_CLIP_LAYER_SHOW_TRACKS") currentData->addClipLayerShowTrack(trackName);
                    else if (key == "ACTION_LAYER_SHOW_TRACKS") currentData->addActionClipLayerShowTrack(trackName);
                }
            }
        } else if (key == "ACTION_ANIM") {
            currentData->setActionAnim(value);
        } else if (key == "ACTION_LAYER_CLIP") {
            currentData->addActionClipLayer(value);
        } else if (key == "CRACKED1_OVERRIDE" || key == "CRACKED2_OVERRIDE") {
            std::stringstream ss(value);
            std::string pairStr;
            while (std::getline(ss, pairStr, ',')) {
                size_t colonPos = pairStr.find(':');
                if (colonPos != std::string::npos) {
                    std::string trackName = pairStr.substr(0, colonPos);
                    std::string texName = pairStr.substr(colonPos + 1);
                    
                    // Trim spaces
                    size_t start = trackName.find_first_not_of(" \t\r\n");
                    if (start != std::string::npos) trackName = trackName.substr(start);
                    size_t end = trackName.find_last_not_of(" \t\r\n");
                    if (end != std::string::npos) trackName = trackName.substr(0, end + 1);
                    
                    start = texName.find_first_not_of(" \t\r\n");
                    if (start != std::string::npos) texName = texName.substr(start);
                    end = texName.find_last_not_of(" \t\r\n");
                    if (end != std::string::npos) texName = texName.substr(0, end + 1);
                    
                    if (key == "CRACKED1_OVERRIDE") currentData->addCracked1Override(trackName, texName);
                    else if (key == "CRACKED2_OVERRIDE") currentData->addCracked2Override(trackName, texName);
                }
            }
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
    creatorMap[CHERRYBOMB] = []() { return std::make_unique<CherryBomb>(); };
    creatorMap[POTATOMINE] = []() { return std::make_unique<PotatoMine>(); };
    creatorMap[CHOMPER] = []() { return std::make_unique<Chomper>(); };
    creatorMap[PUFFSHROOM] = []() { return std::make_unique<PuffShroom>(); };
    creatorMap[SUNSHROOM] = []() { return std::make_unique<SunShroom>(); };
    creatorMap[FUMESHROOM] = []() { return std::make_unique<FumeShroom>(); };
    creatorMap[GRAVEBUSTER] = []() { return std::make_unique<GraveBuster>(); };
    creatorMap[HYPNOSHROOM] = []() { return std::make_unique<HypnoShroom>(); };
    creatorMap[SCAREDYSHROOM] = []() { return std::make_unique<ScaredyShroom>(); };
    creatorMap[ICESHROOM] = []() { return std::make_unique<IceShroom>(); };
    creatorMap[DOOMSHROOM] = []() { return std::make_unique<DoomShroom>(); };
    creatorMap[LILYPAD] = []() { return std::make_unique<LilyPad>(); };
    creatorMap[SQUASH] = []() { return std::make_unique<Squash>(); };
    creatorMap[THREEPEATER] = []() { return std::make_unique<ThreePeater>(); };
    creatorMap[TANGLEKELP] = []() { return std::make_unique<TangleKelp>(); };
    creatorMap[JALAPENO] = []() { return std::make_unique<Jalapeno>(); };
    creatorMap[CALTROP] = []() { return std::make_unique<Caltrop>(); };
    creatorMap[TORCHWOOD] = []() { return std::make_unique<TorchWood>(); };
    creatorMap[TALLNUT] = []() { return std::make_unique<TallNut>(); };
    creatorMap[SEASHROOM] = []() { return std::make_unique<SeaShroom>(); };
    creatorMap[PLANTERN] = []() { return std::make_unique<Plantern>(); };
    creatorMap[BLOVER] = []() { return std::make_unique<Blover>(); };
    creatorMap[SPLITPEA] = []() { return std::make_unique<SplitPea>(); };
    creatorMap[STARFRUIT] = []() { return std::make_unique<Starfruit>(); };
    creatorMap[PUMPKIN] = []() { return std::make_unique<Pumpkin>(); };
    creatorMap[MAGNETSHROOM] = []() { return std::make_unique<MagnetShroom>(); };
    creatorMap[CABBAGEPULT] = []() { return std::make_unique<CabbagePult>(); };
    creatorMap[CORNPULT] = []() { return std::make_unique<CornPult>(); };
    creatorMap[COFFEEBEAN] = []() { return std::make_unique<CoffeeBean>(); };
    creatorMap[GARLIC] = []() { return std::make_unique<Garlic>(); };
    creatorMap[UMBRELLALEAF] = []() { return std::make_unique<UmbrellaLeaf>(); };
    creatorMap[MARIGOLD] = []() { return std::make_unique<Marigold>(); };
    creatorMap[MELONPULT] = []() { return std::make_unique<MelonPult>(); };
    creatorMap[GATLINGPEA] = []() { return std::make_unique<GatlingPea>(); };
    creatorMap[TWINSUNFLOWER] = []() { return std::make_unique<TwinSunflower>(); };
    creatorMap[GLOOMSHROOM] = []() { return std::make_unique<GloomShroom>(); };
    creatorMap[CATTAIL] = []() { return std::make_unique<Cattail>(); };
    creatorMap[SPIKEROCK] = []() { return std::make_unique<SpikeRock>(); };
    creatorMap[GOLDMAGNET] = []() { return std::make_unique<GoldMagnet>(); };
    creatorMap[WINTERMELON] = []() { return std::make_unique<WinterMelon>(); };
    creatorMap[COBCANNON] = []() { return std::make_unique<CobCannon>(); };
    creatorMap[IMITATER] = []() { return std::make_unique<Imitater>(); };
} 

ReanimInstance PlantFactory::createReanim(float scalar, const std::string& packageName, const std::string& animName, const std::string& clipLoopName) {
    ReanimInstance plantReanim;
    plantReanim.setTextureScalar(scalar);
    plantReanim.setTexturePackage(textureManager -> getPackage(packageName));
    plantReanim.setAnimation(animationManager -> getAnimationData(animName));
    plantReanim.setDefaultClip(clipLoopName); 

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

        // Set up main animation from the data-driven reanim metadata
        ReanimInstance mainAnim = createReanim(data->getReanimScalar(), data->getReanimPackage(),
                                               data->getReanimAnim(), data->getReanimClip());
        for (const std::string& trackName : data->getHiddenTracks()) {
            mainAnim.hideTrack(trackName);
        }
        for (const auto& layerPair : data->getClipLayers()) {
            mainAnim.addClipLayer(layerPair.first, layerPair.second);
        }
        plantPtr->setReanimInstance(std::move(mainAnim));
    }

    return plantPtr;
}

int PlantFactory::getSunCost(PlantType pType) const {
    if (!plantDataStorage) return 0;
    PlantData* data = plantDataStorage->getPlantData(pType);
    return data ? data->getSunCost() : 0;
}

float PlantFactory::getSeedRecharge(PlantType pType) const {
    if (!plantDataStorage) return 0.0f;
    PlantData* data = plantDataStorage->getPlantData(pType);
    return data ? data->getSeedRecharge() : 0.0f;
}

std::map<PlantType, int> PlantFactory::getAllSunCosts() const {
    std::map<PlantType, int> result;
    for (PlantType type : getAllPlantTypes()) {
        result[type] = getSunCost(type);
    }
    return result;
}

std::map<PlantType, float> PlantFactory::getAllSeedRecharges() const {
    std::map<PlantType, float> result;
    for (PlantType type : getAllPlantTypes()) result[type] = getSeedRecharge(type);
    return result;
}
