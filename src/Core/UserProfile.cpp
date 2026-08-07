#include "Core/UserProfile.hpp"

namespace {
    std :: string normalizeProfileName(std :: string name) {
        return name.empty() ? "Anonymous" : std :: move(name);
    }
}

UserProfile :: UserProfile(std :: string name) : profileName(normalizeProfileName(std :: move(name))) {
    unlockPlant(PEASHOOTER);
}

bool UserProfile :: isValidPlantType(PlantType type) {
    const int index = static_cast<int>(type);
    return index >= 0 && index < static_cast<int>(PLANT_COUNT);
}

const std :: string& UserProfile :: getProfileName() const {
    return profileName;
}

const LevelID& UserProfile :: getHighestUnlockedLevel() const {
    return highestUnlockedLevel;
}

const std :: bitset<PLANT_COUNT>& UserProfile :: getUnlockedPlants() const {
    return unlockedPlants;
}

bool UserProfile :: isPlantUnlocked(PlantType type) const {
    if(!isValidPlantType(type)) return false;
    return unlockedPlants.test(static_cast<size_t>(type));
}

void UserProfile :: setProfileName(std :: string name) {
    profileName = normalizeProfileName(std :: move(name));
}

void UserProfile :: setHighestUnlockedLevel(LevelID level) {
    highestUnlockedLevel.world = std :: max(1, level.world);
    highestUnlockedLevel.stage = std :: max(1, level.stage);
}

void UserProfile :: setUnlockedPlants(const std :: bitset<PLANT_COUNT>& plants) {
    unlockedPlants = plants;
}

void UserProfile :: unlockPlant(PlantType type) {
    if(!isValidPlantType(type)) return;
    unlockedPlants.set(static_cast<size_t>(type));
}

void UserProfile :: lockPlant(PlantType type) {
    if(!isValidPlantType(type)) return;
    unlockedPlants.reset(static_cast<size_t>(type));
}
