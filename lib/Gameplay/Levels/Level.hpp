#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "Common.hpp"
#include <Gameplay/Levels/WaveConfig.hpp>

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

class Level{
private:
    LevelID id;
    std :: vector<LaneType> lanes;
    WaveList waves;
    bool useSodRollIntro = false;

    void loadFromFile();
    void parseLanes(const std :: string& value);

public:
    explicit Level(LevelID _id);
    const WaveList& getWave() const;
    const std :: vector<LaneType>& getLanes() const;
    bool usesSodRollIntro() const;
};

#endif 
