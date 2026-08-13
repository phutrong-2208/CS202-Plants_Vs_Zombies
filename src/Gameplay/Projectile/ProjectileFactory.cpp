#include <Gameplay/Projectile/ProjectileFactory.hpp>

// Maps section name strings from ProjectileConfig.txt to ProjectileType enum values
static const std::map<std::string, ProjectileType> projectileTypeNameMap = {
    {"PROJECTILE_PEA",     PROJECTILE_PEA},
    {"PROJECTILE_SNOWPEA", PROJECTILE_SNOWPEA},
    {"PROJECTILE_CACTUS",  PROJECTILE_CACTUS},
    {"PROJECTILE_STAR",    PROJECTILE_STAR},
    {"PROJECTILE_PUFF",    PROJECTILE_PUFF},
    {"PROJECTILE_CABBAGE", PROJECTILE_CABBAGE},
    {"PROJECTILE_CORN",    PROJECTILE_CORN},
    {"PROJECTILE_MELON",   PROJECTILE_MELON},
    {"PROJECTILE_WINTERMELON", PROJECTILE_WINTERMELON}
};

/////////////////////////////////////
///     PROJECTILE DATASET        ///
/////////////////////////////////////

ProjectileDataset::ProjectileDataset() {
    std::string configPath = std::string(PROJECT_DIR) + "assets/data/config/ProjectileConfig.txt";
    loadFromFile(configPath);
}

void ProjectileDataset::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[ProjectileDataset] Failed to open: " << filepath << std::endl;
        return;
    }

    std::string line;
    ProjectileType currentType = PROJECTILE_COUNT;
    std::unique_ptr<ProjectileData> currentData = nullptr;

    while (std::getline(file, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);

        // Skip empty lines
        if (line.empty()) continue;

        // Section header: [PROJECTILE_NAME]
        if (line.front() == '[' && line.back() == ']') {
            // Save previous projectile data if any
            if (currentData && currentType != PROJECTILE_COUNT) {
                projectileDatasetMap[currentType] = std::move(currentData);
            }

            std::string sectionName = line.substr(1, line.size() - 2);
            auto it = projectileTypeNameMap.find(sectionName);
            if (it != projectileTypeNameMap.end()) {
                currentType = it->second;
                currentData = std::make_unique<ProjectileData>();
            } else {
                std::cerr << "[ProjectileDataset] Unknown projectile type: " << sectionName << std::endl;
                currentType = PROJECTILE_COUNT;
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

        if (key == "RADIUS") {
            currentData->setRadius(std::stof(value));
        } else if (key == "VELOCITY_X") {
            Vector2 vel = currentData->getVelocity();
            vel.x = std::stof(value);
            currentData->setVelocity(vel);
        } else if (key == "VELOCITY_Y") {
            Vector2 vel = currentData->getVelocity();
            vel.y = std::stof(value);
            currentData->setVelocity(vel);
        } else if (key == "RANGE") {
            currentData->setRange(std::stof(value));
        } else if (key == "TEXTURE_NAME") {
            currentData->setTextureName(value);
        } else if (key == "CHILL_DURATION") {
            currentData->setChillDuration(std::stof(value));
        }
    }

    // Don't forget the last projectile entry
    if (currentData && currentType != PROJECTILE_COUNT) {
        projectileDatasetMap[currentType] = std::move(currentData);
    }

    file.close();
}

ProjectileData* ProjectileDataset::getProjectileData(ProjectileType pType) {
    auto it = projectileDatasetMap.find(pType);
    if (it != projectileDatasetMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

/////////////////////////////////////
///     PROJECTILE FACTORY        ///
/////////////////////////////////////

void ProjectileFactory::setProjectileTexturePackage(TexturePackage* package) {
    projTexturePackage = package;
}

void ProjectileFactory::loadProjectileMechanics() {
    projectileDataStorage = std::make_unique<ProjectileDataset>();
}

ProjectileData* ProjectileFactory::getProjectileData(ProjectileType pType) {
    if (!projectileDataStorage) return nullptr;
    return projectileDataStorage->getProjectileData(pType);
}

Texture2D* ProjectileFactory::getProjectileTexture(ProjectileType pType) {
    if (!projTexturePackage || !projectileDataStorage) return nullptr;

    ProjectileData* data = projectileDataStorage->getProjectileData(pType);
    if (!data) return nullptr;

    return projTexturePackage->GetTexture(data->getTextureName());
}

std::unique_ptr<Projectile> ProjectileFactory::createProjectile(
    ProjectileType pType, Vector2 spawnPos, float damage
) {
    ProjectileData* data = getProjectileData(pType);
    if (!data) return nullptr;

    Texture2D* tex = getProjectileTexture(pType);

    auto proj = std::make_unique<Projectile>(spawnPos, data, damage, data->getRange(), tex);
    proj->setType(pType);
    return proj;
}