#ifndef USER_PROFILE_HPP
#define USER_PROFILE_HPP

#include <bitset>
#include <string>

#include "Common.hpp"
#include "Gameplay/Levels/Level.hpp"
#include "Gameplay/Plants/Plant.hpp"

class UserProfile {
private:
    std :: string profileName = "Anonymous";
    LevelID highestUnlockedLevel{1, 1};
    std :: bitset<PLANT_COUNT> unlockedPlants;

    static bool isValidPlantType(PlantType type);

public:
    UserProfile(std :: string name = "Anonymous");

    const std :: string& getProfileName() const;
    const LevelID& getHighestUnlockedLevel() const;
    const std :: bitset<PLANT_COUNT>& getUnlockedPlants() const;
    bool isPlantUnlocked(PlantType type) const;

    void setProfileName(std :: string name);
    void setHighestUnlockedLevel(LevelID level);
    void setUnlockedPlants(const std :: bitset<PLANT_COUNT>& plants);
    void unlockPlant(PlantType type);
    void lockPlant(PlantType type);
};


#endif
