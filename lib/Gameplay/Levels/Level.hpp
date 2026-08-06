#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "Common.hpp"
#include <Gameplay/Levels/WaveConfig.hpp>
#include <Gameplay/Plants/Plant.hpp>

struct LevelID {
    int world = 1;
    int stage = 1;

    std :: string getFileName(void) const{
        return "Level_" + std :: to_string(world) + "_" + std :: to_string(stage) + ".txt";
    }
};

enum class LaneType{
    INACTIVE, 
    POOL,
    ROOF,
    GRASS
};

enum class MapType {
    DAY,
    NIGHT,
    POOL,
    FOG,
    ROOF,
    UNKNOWN
};

enum class Section{
    NONE,
    LEVEL,
    PLANTS,
    WAVE
};


class Level{
private:
    LevelID id;
    int formatVersion = 1;
    MapType mapType = MapType :: DAY;

    int startingSun = 50;
    int seedSlots = 6;
    bool allowPlantChooser = true;
    bool useSodRollIntro = false;

    std :: vector<LaneType> lanes;
    std :: vector<PlantType> availablePlants;
    std :: vector<PlantType> preselectedPlants;
    PlantType rewardPlant = PLANT_COUNT;

    WaveList waves;
    float firstWaveDelay = 18.0f;
    float betweenWaveDelay = 10.0f;

    void loadFromFile();
    void parseLanes(const std :: string& value);
    void parsePlants(
        const std :: string& value,
        std :: vector<PlantType>& destination
    );
    void parseZombies(const std :: string& value, WaveConfig& wave);

public:
    explicit Level(LevelID _id);

    const LevelID& getID() const;
    int getFormatVersion() const;
    MapType getMapType() const;
    int getStartingSun() const;
    int getSeedSlots() const;
    bool allowsPlantChooser() const;
    bool usesSodRollIntro() const;

    const std :: vector<LaneType>& getLanes() const;
    const std :: vector<PlantType>& getAvailablePlants() const;
    const std :: vector<PlantType>& getPreselectedPlants() const;
    PlantType getRewardPlant() const;

    const WaveList& getWaves() const;
    float getFirstWaveDelay() const;
    float getBetweenWaveDelay() const;
};

#endif 
